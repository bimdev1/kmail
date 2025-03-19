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

namespace KMail {

LocalAIService::LocalAIService(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_initialized(false)
{
    // Initialize category map
    m_categoryMap[QStringLiteral("urgent")] = Urgent;
    m_categoryMap[QStringLiteral("follow-up")] = FollowUp;
    m_categoryMap[QStringLiteral("low")] = LowPriority;
    m_categoryMap[QStringLiteral("uncategorized")] = Uncategorized;

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

    QNetworkRequest request(QUrl(QStringLiteral("https://api.deepseek.com/v1/chat/completions")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setRawHeader("Authorization", QByteArray("Bearer ") + m_apiKey.toUtf8());

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

    QNetworkReply *reply = m_networkManager->post(request, jsonData);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        
        if (reply->error() != QNetworkReply::NoError) {
            emit error(reply->errorString());
            return;
        }

        QJsonDocument response = QJsonDocument::fromJson(reply->readAll());
        QJsonObject obj = response.object();
        
        if (obj.contains(QStringLiteral("error"))) {
            emit error(obj[QStringLiteral("error")].toObject()[QStringLiteral("message")].toString());
            return;
        }

        QString generatedText = obj[QStringLiteral("choices")].toArray()[0].toObject()[QStringLiteral("message")]
                                 .toObject()[QStringLiteral("content")].toString();
        emit replyGenerated(generatedText);
    });
}

EmailCategory LocalAIService::categorizeEmail(const QString &emailContent)
{
    if (!m_initialized && !initialize()) {
        return Uncategorized;
    }

    // Create a temporary file with the email content
    QTemporaryFile tempFile;
    if (!tempFile.open()) {
        qCWarning(KMAIL_LOG) << "Failed to create temporary file for email content";
        return Uncategorized;
    }

    QTextStream stream(&tempFile);
    stream << emailContent;
    stream.flush();

    // Call the Python script
    QStringList args;
    args << tempFile.fileName();
    QString result = callPythonScript(QStringLiteral("ai_email_categorizer.py"), args);

    // Parse the result
    result = result.trimmed().toLower();
    if (m_categoryMap.contains(result)) {
        EmailCategory category = m_categoryMap[result];
        Q_EMIT emailCategorized(category);
        return category;
    }

    return Uncategorized;
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

QStringList LocalAIService::extractTasks(const QString &emailContent)
{
    if (!m_initialized && !initialize()) {
        return QStringList();
    }

    // Create a temporary file with the email content
    QTemporaryFile tempFile;
    if (!tempFile.open()) {
        qCWarning(KMAIL_LOG) << "Failed to create temporary file for email content";
        return QStringList();
    }

    QTextStream stream(&tempFile);
    stream << emailContent;
    stream.flush();

    // Call the Python script
    QStringList args;
    args << tempFile.fileName();
    QString result = callPythonScript(QStringLiteral("ai_task_extractor.py"), args);

    // Parse the result
    QStringList tasks = result.split(QStringLiteral("\n"), Qt::SkipEmptyParts);
    Q_EMIT tasksExtracted(tasks);
    return tasks;
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
    QString summary = callPythonScript(QStringLiteral("ai_email_summarizer.py"), args);

    Q_EMIT emailSummarized(summary);
    return summary;
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

} // namespace KMail
