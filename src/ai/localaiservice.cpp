/*
 * This file is part of KMail, the KDE mail client.
 * SPDX-FileCopyrightText: 2025 KMail AI Integration Team
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "localaiservice.h"
#include "kmail_debug.h"

#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QTextStream>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSettings>
#include <QTimer>
#include <QCache>
#include <QCryptographicHash>

namespace KMail {

const int MAX_RETRIES = 3;
const int RETRY_DELAY_MS = 500;
const int DEFAULT_CACHE_SIZE = 100;
const int CACHE_TTL_MS = 300000; // 5 minutes

struct PendingRequest {
    QString endpoint;
    QByteArray data;
    int retryCount = 0;
    QTimer *retryTimer = nullptr;
};

struct CacheEntry {
    QString result;
    qint64 timestamp;
};

LocalAIService::LocalAIService(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_cache(DEFAULT_CACHE_SIZE)
{
    // Initialize category map
    m_categoryMap[QStringLiteral("urgent")] = Urgent;
    m_categoryMap[QStringLiteral("normal")] = Normal;
    m_categoryMap[QStringLiteral("low")] = Low;

    loadApiKey();
}

void LocalAIService::loadApiKey()
{
    QSettings settings(QStringLiteral("KMail"), QStringLiteral("AI"));
    m_apiKey = settings.value(QStringLiteral("DeepSeekApiKey")).toString();
}

void LocalAIService::setApiKey(const QString &apiKey)
{
    QSettings settings(QStringLiteral("KMail"), QStringLiteral("AI"));
    settings.setValue(QStringLiteral("DeepSeekApiKey"), apiKey);
    m_apiKey = apiKey;
}

bool LocalAIService::initialize()
{
    if (m_initialized) {
        return true;
    }

    // Check if Python is available
    if (!isPythonAvailable()) {
        qCWarning(KMAIL_LOG) << "Python is not available. AI features will be disabled.";
        return false;
    }

    // Check if scripts directory exists
    QDir scriptsDir(pythonScriptsPath());
    if (!scriptsDir.exists()) {
        if (!scriptsDir.mkpath(QStringLiteral("."))) {
            qCWarning(KMAIL_LOG) << "Failed to create scripts directory:" << pythonScriptsPath();
            return false;
        }
    }

    // Create Python scripts if they don't exist
    createPythonScripts();

    m_initialized = true;
    return true;
}

void LocalAIService::generateReply(const QString &emailContent, const QString &prompt)
{
    if (m_apiKey.isEmpty()) {
        emit error(tr("DeepSeek API key not set. Please configure it in the settings."));
        return;
    }

    QJsonObject data;
    data["model"] = QStringLiteral("deepseek-chat");
    
    QJsonArray messages;
    QJsonObject systemMsg;
    systemMsg["role"] = QStringLiteral("system");
    systemMsg["content"] = QStringLiteral("You are an AI assistant helping to draft email replies. Be professional and concise.");
    messages.append(systemMsg);

    QJsonObject contextMsg;
    contextMsg["role"] = QStringLiteral("user");
    contextMsg["content"] = QStringLiteral("Original email:\n") + emailContent + QStringLiteral("\n\nInstructions: ") + prompt;
    messages.append(contextMsg);

    data["messages"] = messages;
    data["temperature"] = 0.7;
    data["max_tokens"] = 1000;

    QJsonDocument doc(data);
    QByteArray jsonData = doc.toJson();

    makeRequest(QStringLiteral("chat/completions"), jsonData);
}

EmailCategory LocalAIService::categorizeEmail(const QString &emailContent)
{
    if (m_apiKey.isEmpty()) {
        emit error(tr("DeepSeek API key not set. Please configure it in the settings."));
        return Uncategorized;
    }

    QJsonObject data;
    data["model"] = QStringLiteral("deepseek-chat");
    
    QJsonArray messages;
    QJsonObject systemMsg;
    systemMsg["role"] = QStringLiteral("system");
    systemMsg["content"] = QStringLiteral("You are an AI assistant that categorizes emails. "
                                        "Categorize the email into one of these categories: "
                                        "urgent, important, follow-up, low, or uncategorized. "
                                        "Respond with ONLY the category name in lowercase.");
    messages.append(systemMsg);

    QJsonObject contextMsg;
    contextMsg["role"] = QStringLiteral("user");
    contextMsg["content"] = emailContent;
    messages.append(contextMsg);

    data["messages"] = messages;
    data["temperature"] = 0.3; // Lower temperature for more consistent categorization
    data["max_tokens"] = 10;   // We only need a single word response

    QJsonDocument doc(data);
    QByteArray jsonData = doc.toJson();

    makeRequest(QStringLiteral("chat/completions"), jsonData);

    return Uncategorized;
}

void LocalAIService::makeRequest(const QString &endpoint, const QByteArray &data)
{
    // Check cache first
    QString cacheKey = generateCacheKey(endpoint, data);
    QString cachedResponse = getCachedResponse(cacheKey);
    
    if (!cachedResponse.isEmpty()) {
        // Process cached response
        if (endpoint.contains(QStringLiteral("categorize"))) {
            emit emailCategorized(cachedResponse.contains(QStringLiteral("urgent"), Qt::CaseInsensitive) ? 
                                EmailCategory::Urgent : EmailCategory::Normal);
        } else if (endpoint.contains(QStringLiteral("reply"))) {
            emit replyGenerated(cachedResponse);
        } else if (endpoint.contains(QStringLiteral("follow-up"))) {
            emit followUpDetected(cachedResponse.trimmed().toLower() == QStringLiteral("true"));
        } else if (endpoint.contains(QStringLiteral("tasks"))) {
            emit tasksExtracted(cachedResponse.split('\n', Qt::SkipEmptyParts));
        } else if (endpoint.contains(QStringLiteral("summarize"))) {
            emit emailSummarized(cachedResponse);
        }
        return;
    }

    QNetworkRequest request(QUrl(QStringLiteral("https://api.deepseek.com/v1/") + endpoint));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setRawHeader("Authorization", QByteArray("Bearer ") + m_apiKey.toUtf8());

    QNetworkReply *reply = m_networkManager->post(request, data);
    
    PendingRequest pendingRequest;
    pendingRequest.endpoint = endpoint;
    pendingRequest.data = data;
    m_pendingRequests.insert(reply, pendingRequest);

    connect(reply, &QNetworkReply::finished, this, [this, reply, cacheKey]() {
        if (reply->error() != QNetworkReply::NoError) {
            handleNetworkError(reply, m_pendingRequests[reply]);
            return;
        }

        QJsonDocument response = QJsonDocument::fromJson(reply->readAll());
        QJsonObject obj = response.object();
        
        if (obj.contains(QStringLiteral("error"))) {
            QString errorMsg = obj[QStringLiteral("error")].toObject()[QStringLiteral("message")].toString();
            handleNetworkError(reply, m_pendingRequests[reply]);
            return;
        }

        // Process successful response
        QString result = obj[QStringLiteral("choices")].toArray()[0].toObject()[QStringLiteral("message")]
                          .toObject()[QStringLiteral("content")].toString();

        // Cache the response
        cacheResponse(cacheKey, result);

        cleanupRequest(m_pendingRequests[reply]);
        reply->deleteLater();

        // Emit appropriate signal based on the endpoint
        if (m_pendingRequests[reply].endpoint.contains(QStringLiteral("categorize"))) {
            emit emailCategorized(result.contains(QStringLiteral("urgent"), Qt::CaseInsensitive) ? 
                                EmailCategory::Urgent : EmailCategory::Normal);
        } else if (m_pendingRequests[reply].endpoint.contains(QStringLiteral("reply"))) {
            emit replyGenerated(result);
        } else if (m_pendingRequests[reply].endpoint.contains(QStringLiteral("follow-up"))) {
            emit followUpDetected(result.trimmed().toLower() == QStringLiteral("true"));
        } else if (m_pendingRequests[reply].endpoint.contains(QStringLiteral("tasks"))) {
            emit tasksExtracted(result.split('\n', Qt::SkipEmptyParts));
        } else if (m_pendingRequests[reply].endpoint.contains(QStringLiteral("summarize"))) {
            emit emailSummarized(result);
        }
    });
}

void LocalAIService::handleNetworkError(QNetworkReply *reply, const PendingRequest &request)
{
    if (request.retryCount < MAX_RETRIES) {
        retryRequest(request);
    } else {
        emit error(tr("Failed to connect to DeepSeek API after %1 attempts: %2")
                  .arg(MAX_RETRIES + 1)
                  .arg(reply->errorString()));
        cleanupRequest(request);
    }
    reply->deleteLater();
}

void LocalAIService::retryRequest(const PendingRequest &request)
{
    PendingRequest newRequest = request;
    newRequest.retryCount++;
    
    // Exponential backoff: delay = initial_delay * (2 ^ retry_count)
    int delay = RETRY_DELAY_MS * (1 << newRequest.retryCount);
    
    newRequest.retryTimer = new QTimer(this);
    newRequest.retryTimer->setSingleShot(true);
    connect(newRequest.retryTimer, &QTimer::timeout, this, [this, newRequest]() {
        makeRequest(newRequest.endpoint, newRequest.data);
    });
    
    newRequest.retryTimer->start(delay);
}

void LocalAIService::cleanupRequest(const PendingRequest &request)
{
    delete request.retryTimer;
    m_pendingRequests.remove(m_pendingRequests.key(request));
}

LocalAIService::~LocalAIService()
{
    // Clean up any pending requests
    for (const auto &request : m_pendingRequests) {
        cleanupRequest(request);
    }
}

QString LocalAIService::generateReply(const QString &emailContent, const QStringList &userHistory)
{
    if (!m_initialized && !initialize()) {
        return QString();
    }

    // Create a temporary file with the email content
    QTemporaryFile emailFile;
    if (!emailFile.open()) {
        qCWarning(KMAIL_LOG) << "Failed to create temporary file for email content";
        return QString();
    }

    QTextStream emailStream(&emailFile);
    emailStream << emailContent;
    emailStream.flush();

    // Create a temporary file with the user history
    QTemporaryFile historyFile;
    if (!historyFile.open()) {
        qCWarning(KMAIL_LOG) << "Failed to create temporary file for user history";
        return QString();
    }

    QTextStream historyStream(&historyFile);
    for (const QString &email : userHistory) {
        historyStream << email << "\n---\n";
    }
    historyStream.flush();

    // Call the Python script
    QStringList args;
    args << emailFile.fileName() << historyFile.fileName();
    QString reply = callPythonScript(QStringLiteral("ai_reply_generator.py"), args);

    Q_EMIT replyGenerated(reply);
    return reply;
}

void LocalAIService::extractTasks(const QString &emailContent)
{
    if (m_apiKey.isEmpty()) {
        emit error(tr("DeepSeek API key not set. Please configure it in the settings."));
        return;
    }

    QNetworkRequest request(QUrl(QStringLiteral("https://api.deepseek.com/v1/chat/completions")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setRawHeader("Authorization", QByteArray("Bearer ") + m_apiKey.toUtf8());

    QJsonObject data;
    data["model"] = QStringLiteral("deepseek-chat");
    
    QJsonArray messages;
    QJsonObject systemMsg;
    systemMsg["role"] = QStringLiteral("system");
    systemMsg["content"] = QStringLiteral("You are an AI assistant that extracts actionable tasks from emails. "
                                        "Extract tasks and format them as a JSON array of strings. "
                                        "Each task should be clear and actionable. "
                                        "If no tasks are found, return an empty array.");
    messages.append(systemMsg);

    QJsonObject contextMsg;
    contextMsg["role"] = QStringLiteral("user");
    contextMsg["content"] = emailContent;
    messages.append(contextMsg);

    data["messages"] = messages;
    data["temperature"] = 0.3;
    data["max_tokens"] = 500;

    QJsonDocument doc(data);
    QByteArray jsonData = doc.toJson();

    makeRequest(QStringLiteral("chat/completions"), jsonData);
}

bool LocalAIService::needsFollowUp(const QString &emailContent)
{
    if (!m_initialized && !initialize()) {
        return false;
    }

    // Create a temporary file with the email content
    QTemporaryFile tempFile;
    if (!tempFile.open()) {
        qCWarning(KMAIL_LOG) << "Failed to create temporary file for email content";
        return false;
    }

    QTextStream stream(&tempFile);
    stream << emailContent;
    stream.flush();

    // Call the Python script
    QStringList args;
    args << tempFile.fileName();
    QString result = callPythonScript(QStringLiteral("ai_followup_detector.py"), args);

    // Parse the result
    return result.trimmed().toLower() == QStringLiteral("true");
}

void LocalAIService::detectFollowUp(const QString &emailContent)
{
    if (m_apiKey.isEmpty()) {
        emit error(tr("DeepSeek API key not set. Please configure it in the settings."));
        return;
    }

    QNetworkRequest request(QUrl(QStringLiteral("https://api.deepseek.com/v1/chat/completions")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setRawHeader("Authorization", QByteArray("Bearer ") + m_apiKey.toUtf8());

    QJsonObject data;
    data["model"] = QStringLiteral("deepseek-chat");
    
    QJsonArray messages;
    QJsonObject systemMsg;
    systemMsg["role"] = QStringLiteral("system");
    systemMsg["content"] = QStringLiteral("You are an AI assistant that determines if an email needs a follow-up. "
                                        "Analyze the email and respond with ONLY 'true' if it needs follow-up "
                                        "or 'false' if it doesn't. Consider phrases like 'please respond', "
                                        "'let me know', 'waiting for your reply', etc.");
    messages.append(systemMsg);

    QJsonObject contextMsg;
    contextMsg["role"] = QStringLiteral("user");
    contextMsg["content"] = emailContent;
    messages.append(contextMsg);

    data["messages"] = messages;
    data["temperature"] = 0.1; // Very low temperature for consistent yes/no decisions
    data["max_tokens"] = 10;

    QJsonDocument doc(data);
    QByteArray jsonData = doc.toJson();

    makeRequest(QStringLiteral("chat/completions"), jsonData);
}

QString LocalAIService::suggestFollowUpDate(const QString &emailContent)
{
    if (!m_initialized && !initialize()) {
        return QString();
    }

    // Create a temporary file with the email content
    QTemporaryFile tempFile;
    if (!tempFile.open()) {
        qCWarning(KMAIL_LOG) << "Failed to create temporary file for email content";
        return QString();
    }

    QTextStream stream(&tempFile);
    stream << emailContent;
    stream.flush();

    // Call the Python script
    QStringList args;
    args << tempFile.fileName();
    QString date = callPythonScript(QStringLiteral("ai_followup_date_suggester.py"), args);

    Q_EMIT followUpSuggested(date);
    return date;
}

QString LocalAIService::summarizeEmail(const QString &emailContent)
{
    if (m_apiKey.isEmpty()) {
        emit error(tr("DeepSeek API key not set. Please configure it in the settings."));
        return;
    }

    QNetworkRequest request(QUrl(QStringLiteral("https://api.deepseek.com/v1/chat/completions")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setRawHeader("Authorization", QByteArray("Bearer ") + m_apiKey.toUtf8());

    QJsonObject data;
    data["model"] = QStringLiteral("deepseek-chat");
    
    QJsonArray messages;
    QJsonObject systemMsg;
    systemMsg["role"] = QStringLiteral("system");
    systemMsg["content"] = QStringLiteral("You are an AI assistant that summarizes emails. "
                                        "Create a concise summary that captures the main points "
                                        "and any important action items. Keep the summary brief "
                                        "but informative.");
    messages.append(systemMsg);

    QJsonObject contextMsg;
    contextMsg["role"] = QStringLiteral("user");
    contextMsg["content"] = emailContent;
    messages.append(contextMsg);

    data["messages"] = messages;
    data["temperature"] = 0.5;
    data["max_tokens"] = 200;

    QJsonDocument doc(data);
    QByteArray jsonData = doc.toJson();

    makeRequest(QStringLiteral("chat/completions"), jsonData);

    return QString();
}

QString LocalAIService::callPythonScript(const QString &scriptName, const QStringList &args)
{
    QProcess process;
    process.setProgram(QStringLiteral("python3"));

    QStringList processArgs;
    processArgs << QDir(pythonScriptsPath()).filePath(scriptName);
    processArgs << args;
    process.setArguments(processArgs);

    process.start();
    if (!process.waitForStarted(5000)) {
        qCWarning(KMAIL_LOG) << "Failed to start Python script:" << scriptName;
        return QString();
    }

    if (!process.waitForFinished(30000)) {
        qCWarning(KMAIL_LOG) << "Python script timed out:" << scriptName;
        process.kill();
        return QString();
    }

    if (process.exitCode() != 0) {
        qCWarning(KMAIL_LOG) << "Python script failed:" << scriptName;
        qCWarning(KMAIL_LOG) << "Error:" << process.readAllStandardError();
        return QString();
    }

    return QString::fromUtf8(process.readAllStandardOutput());
}

QString LocalAIService::pythonScriptsPath() const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/ai_scripts");
}

bool LocalAIService::isPythonAvailable() const
{
    QProcess process;
    process.setProgram(QStringLiteral("python3"));
    process.setArguments(QStringList() << QStringLiteral("--version"));
    process.start();
    return process.waitForStarted(5000) && process.waitForFinished(5000) && process.exitCode() == 0;
}

void LocalAIService::createPythonScripts()
{
    // Create directory if it doesn't exist
    QDir dir(pythonScriptsPath());
    if (!dir.exists()) {
        dir.mkpath(QStringLiteral("."));
    }

    // Create the email categorizer script
    QFile categorizerFile(dir.filePath(QStringLiteral("ai_email_categorizer.py")));
    if (categorizerFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&categorizerFile);
        stream << R"(#!/usr/bin/env python3
import sys
import re
import json

def categorize_email(email_text):
    """Categorize an email based on its content"""
    # This is a simple rule-based categorization
    # In a real implementation, this would use a machine learning model
    
    email_lower = email_text.lower()
    
    # Check for urgent keywords
    urgent_keywords = ['urgent', 'asap', 'emergency', 'immediately', 'critical', 'important']
    for keyword in urgent_keywords:
        if keyword in email_lower:
            return "urgent"
    
    # Check for follow-up indicators
    followup_keywords = ['follow up', 'follow-up', 'get back to me', 'let me know', 
        'waiting for your response', 'please respond', 'awaiting your reply',
        'looking forward to hearing from you', 'please confirm']
    for keyword in followup_keywords:
        if keyword in email_lower:
            return "follow-up"
    
    # Check for low priority indicators
    low_keywords = ['fyi', 'for your information', 'no rush', 'when you have time', 'not urgent']
    for keyword in low_keywords:
        if keyword in email_lower:
            return "low"
    
    return "uncategorized"

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python ai_email_categorizer.py <email_file>")
        sys.exit(1)
    
    email_file = sys.argv[1]
    with open(email_file, 'r') as f:
        email_text = f.read()
    
    category = categorize_email(email_text)
    print(category)
)";
    }
    categorizerFile.close();

    // Create the reply generator script
    QFile replyGeneratorFile(dir.filePath(QStringLiteral("ai_reply_generator.py")));
    if (replyGeneratorFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&replyGeneratorFile);
        stream << R"(#!/usr/bin/env python3
import sys
import re
import json

def generate_reply(email_text, user_history):
    """Generate a reply to an email based on its content and user history"""
    # This is a simple rule-based reply generator
    # In a real implementation, this would use a language model
    
    email_lower = email_text.lower()
    
    # Extract greeting style from user history
    greeting = "Hello,"
    closing = "Best regards,"
    
    if user_history:
        # Look for greeting patterns in user history
        greeting_patterns = [
            r'(hi|hello|hey|dear).*?,',
            r'(good morning|good afternoon|good evening).*?,',
        ]
        
        closing_patterns = [
            r'(regards|best|sincerely|thank you|thanks).*,',
            r'(cheers|yours truly|yours sincerely|best wishes).*,',
        ]
        
        for email in user_history:
            for pattern in greeting_patterns:
                match = re.search(pattern, email, re.IGNORECASE)
                if match:
                    greeting = match.group(0)
                    break
            
            for pattern in closing_patterns:
                match = re.search(pattern, email, re.IGNORECASE)
                if match:
                    closing = match.group(0)
                    break
    
    # Generate a simple reply based on the email content
    if "meeting" in email_lower or "schedule" in email_lower:
        body = "Thank you for your email about the meeting. I'll check my schedule and get back to you soon."
    elif "question" in email_lower or "help" in email_lower:
        body = "Thank you for your question. I'll look into this and provide you with an answer as soon as possible."
    elif "update" in email_lower or "status" in email_lower:
        body = "Thank you for your email. I'm working on this and will provide an update soon."
    else:
        body = "Thank you for your email. I'll review the information and respond accordingly."
    
    reply = f"{greeting}\n\n{body}\n\n{closing}"
    return reply

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python ai_reply_generator.py <email_file> <history_file>")
        sys.exit(1)
    
    email_file = sys.argv[1]
    history_file = sys.argv[2]
    
    with open(email_file, 'r') as f:
        email_text = f.read()
    
    with open(history_file, 'r') as f:
        user_history = f.read().split('---')
    
    reply = generate_reply(email_text, user_history)
    print(reply)
)";
    }
    replyGeneratorFile.close();

    // Create the task extractor script
    QFile taskExtractorFile(dir.filePath(QStringLiteral("ai_task_extractor.py")));
    if (taskExtractorFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&taskExtractorFile);
        stream << R"(#!/usr/bin/env python3
import sys
import re
import json

def extract_tasks(email_text):
    """Extract tasks from an email"""
    # This is a simple rule-based task extractor
    # In a real implementation, this would use a named entity recognition model
    
    tasks = []
    
    # Look for common task patterns
    task_patterns = [
        r'(?:please|pls|kindly|can you|could you)\s+(.*?)\??$',
        r'(?:need to|have to|must|should)\s+(.*?)\.$',
        r'(?:todo|to-do|task):\s*(.*?)\.$',
    ]
    
    lines = email_text.split('\n')
    for line in lines:
        line = line.strip()
        if not line:
            continue
        
        for pattern in task_patterns:
            matches = re.finditer(pattern, line, re.IGNORECASE)
            for match in matches:
                task = match.group(1).strip()
                if task and len(task) > 5:  # Avoid very short matches
                    tasks.append(task)
    
    return tasks

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python ai_task_extractor.py <email_file>")
        sys.exit(1)
    
    email_file = sys.argv[1]
    with open(email_file, 'r') as f:
        email_text = f.read()
    
    tasks = extract_tasks(email_text)
    for task in tasks:
        print(task)
)";
    }
    taskExtractorFile.close();

    // Create the follow-up detector script
    QFile followupDetectorFile(dir.filePath(QStringLiteral("ai_followup_detector.py")));
    if (followupDetectorFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&followupDetectorFile);
        stream << R"(#!/usr/bin/env python3
import sys
import re
import json

def needs_followup(email_text):
    """Determine if an email needs a follow-up"""
    # This is a simple rule-based follow-up detector
    # In a real implementation, this would use a machine learning model
    
    email_lower = email_text.lower()
    
    # Check for follow-up indicators
    followup_keywords = [
        'follow up', 'follow-up', 'get back to me', 'let me know', 
        'waiting for your response', 'please respond', 'awaiting your reply',
        'looking forward to hearing from you', 'please confirm'
    ]
    
    for keyword in followup_keywords:
        if keyword in email_lower:
            return True
    
    # Check for question marks
    if '?' in email_text:
        return True
    
    return False

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python ai_followup_detector.py <email_file>")
        sys.exit(1)
    
    email_file = sys.argv[1]
    with open(email_file, 'r') as f:
        email_text = f.read()
    
    needs_followup_result = needs_followup(email_text)
    print(str(needs_followup_result).lower())
)";
    }
    followupDetectorFile.close();

    // Create the follow-up date suggester script
    QFile followupDateSuggesterFile(dir.filePath(QStringLiteral("ai_followup_date_suggester.py")));
    if (followupDateSuggesterFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&followupDateSuggesterFile);
        stream << R"(#!/usr/bin/env python3
import sys
import re
import json
import datetime

def suggest_followup_date(email_text):
    """Suggest a follow-up date for an email"""
    # This is a simple rule-based follow-up date suggester
    # In a real implementation, this would use a more sophisticated algorithm
    
    email_lower = email_text.lower()
    
    # Check for explicit date mentions
    date_patterns = [
        r'by\s+(monday|tuesday|wednesday|thursday|friday|saturday|sunday)',
        r'by\s+(\d{1,2}(?:st|nd|rd|th)?\s+(?:january|february|march|april|may|june|july|august|september|october|november|december))',
        r'by\s+(\d{1,2}/\d{1,2}(?:/\d{2,4})?)',
    ]
    
    for pattern in date_patterns:
        match = re.search(pattern, email_lower)
        if match:
            # In a real implementation, we would parse this date properly
            # For now, just return a date 3 days from now
            future_date = datetime.datetime.now() + datetime.timedelta(days=3)
            return future_date.strftime('%Y-%m-%d')
    
    # Check for urgency indicators
    urgent_keywords = ['urgent', 'asap', 'emergency', 'immediately', 'critical', 'important']
    for keyword in urgent_keywords:
        if keyword in email_lower:
            # For urgent emails, suggest tomorrow
            tomorrow = datetime.datetime.now() + datetime.timedelta(days=1)
            return tomorrow.strftime('%Y-%m-%d')
    
    # Default: suggest 3 business days from now
    future_date = datetime.datetime.now() + datetime.timedelta(days=3)
    return future_date.strftime('%Y-%m-%d')

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python ai_followup_date_suggester.py <email_file>")
        sys.exit(1)
    
    email_file = sys.argv[1]
    with open(email_file, 'r') as f:
        email_text = f.read()
    
    followup_date = suggest_followup_date(email_text)
    print(followup_date)
)";
    }
    followupDateSuggesterFile.close();

    // Create the email summarizer script
    QFile emailSummarizerFile(dir.filePath(QStringLiteral("ai_email_summarizer.py")));
    if (emailSummarizerFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&emailSummarizerFile);
        stream << R"(#!/usr/bin/env python3
import sys
import re
import json

def summarize_email(email_text):
    """Summarize an email"""
    # This is a simple rule-based email summarizer
    # In a real implementation, this would use a language model
    
    # Extract the most important sentences
    sentences = re.split(r'[.!?]', email_text)
    sentences = [s.strip() for s in sentences if s.strip()]
    
    if not sentences:
        return "No content to summarize."
    
    # Simple heuristic: take the first sentence and any sentence with important keywords
    important_keywords = ['important', 'urgent', 'critical', 'deadline', 'meeting', 'decision', 'action']
    
    summary_sentences = [sentences[0]]  # Always include the first sentence
    
    for sentence in sentences[1:]:
        for keyword in important_keywords:
            if keyword in sentence.lower():
                summary_sentences.append(sentence)
                break
    
    # Limit to 3 sentences
    summary_sentences = summary_sentences[:3]
    
    return '. '.join(summary_sentences) + '.'

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python ai_email_summarizer.py <email_file>")
        sys.exit(1)
    
    email_file = sys.argv[1]
    with open(email_file, 'r') as f:
        email_text = f.read()
    
    summary = summarize_email(email_text)
    print(summary)
)";
    }
    emailSummarizerFile.close();

    // Make the scripts executable
    QStringList scripts = {
        QStringLiteral("ai_email_categorizer.py"),
        QStringLiteral("ai_reply_generator.py"),
        QStringLiteral("ai_task_extractor.py"),
        QStringLiteral("ai_followup_detector.py"),
        QStringLiteral("ai_followup_date_suggester.py"),
        QStringLiteral("ai_email_summarizer.py")
    };

    for (const QString &script : scripts) {
        QFile file(dir.filePath(script));
        file.setPermissions(file.permissions() | QFile::ExeOwner | QFile::ExeUser | QFile::ExeGroup | QFile::ExeOther);
    }
}

void LocalAIService::setCacheSize(int maxSize)
{
    m_cache.setMaxCost(maxSize);
}

void LocalAIService::clearCache()
{
    m_cache.clear();
}

QString LocalAIService::getCachedResponse(const QString &key) const
{
    if (auto *entry = m_cache.object(key)) {
        // Check if the entry has expired
        if (QDateTime::currentMSecsSinceEpoch() - entry->timestamp <= CACHE_TTL_MS) {
            return entry->result;
        }
        // Entry has expired, remove it
        m_cache.remove(key);
    }
    return QString();
}

void LocalAIService::cacheResponse(const QString &key, const QString &response)
{
    auto *entry = new CacheEntry{response, QDateTime::currentMSecsSinceEpoch()};
    m_cache.insert(key, entry);
}

QString LocalAIService::generateCacheKey(const QString &endpoint, const QByteArray &data) const
{
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(endpoint.toUtf8());
    hash.addData(data);
    return QString::fromLatin1(hash.result().toHex());
}

} // namespace KMail
