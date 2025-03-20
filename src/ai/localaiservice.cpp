/*
 * This file is part of KMail, the KDE mail client.
 * SPDX-FileCopyrightText: 2025 KMail AI Integration Team
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "localaiservice.h"
#include <KLocalizedString>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QUrlQuery>

namespace KMail {

LocalAIService::LocalAIService(QObject* parent)
    : QObject(parent), m_networkManager(new QNetworkAccessManager(this)), m_retryTimer(new QTimer(this)),
      m_cache(new QCache<QString, QString>(1000))
{
    m_retryTimer->setSingleShot(true);
    connect(m_retryTimer, &QTimer::timeout, this, &LocalAIService::retryRequest);
}

LocalAIService::~LocalAIService()
{
    delete m_cache;
}

bool LocalAIService::initialize()
{
    if (m_apiKey.isEmpty()) {
        emit error(i18n("DeepSeek API key not set. Please configure it in settings."));
        return false;
    }
    return true;
}

void LocalAIService::setApiKey(const QString& apiKey)
{
    m_apiKey = apiKey;
}

void LocalAIService::generateReply(const QString& emailContent, const QString& prompt)
{
    QString cacheKey = QString("reply_%1_%2").arg(emailContent.left(100), prompt.left(50));
    if (auto* cachedReply = m_cache->object(cacheKey)) {
        emit replyGenerated(*cachedReply);
        return;
    }

    QJsonObject requestData;
    requestData["model"] = "deepseek-chat";
    requestData["messages"] = QJsonArray{
        QJsonObject{
            {"role", "system"},
            {"content",
             "You are an email assistant. Generate a professional reply based on the email content and prompt."}},
        QJsonObject{{"role", "user"}, {"content", QString("Email: %1\nPrompt: %2").arg(emailContent, prompt)}}};

    makeRequest("https://api.deepseek.com/v1/chat/completions", QJsonDocument(requestData).toJson(),
                [this, cacheKey](const QString& response) {
                    m_cache->insert(cacheKey, new QString(response));
                    emit replyGenerated(response);
                });
}

void LocalAIService::categorizeEmail(const QString& emailContent)
{
    QString cacheKey = QString("category_%1").arg(emailContent.left(100));
    if (auto* cachedCategory = m_cache->object(cacheKey)) {
        emit emailCategorized(*cachedCategory);
        return;
    }

    QJsonObject requestData;
    requestData["model"] = "deepseek-chat";
    requestData["messages"] = QJsonArray{
        QJsonObject{
            {"role", "system"},
            {"content", "You are an email categorizer. Categorize the email as: Urgent, Follow-Up, or Low Priority."}},
        QJsonObject{{"role", "user"}, {"content", emailContent}}};

    makeRequest("https://api.deepseek.com/v1/chat/completions", QJsonDocument(requestData).toJson(),
                [this, cacheKey](const QString& response) {
                    m_cache->insert(cacheKey, new QString(response));
                    emit emailCategorized(response);
                });
}

void LocalAIService::extractTasks(const QString& emailContent)
{
    QString cacheKey = QString("tasks_%1").arg(emailContent.left(100));
    if (auto* cachedTasks = m_cache->object(cacheKey)) {
        emit tasksExtracted(*cachedTasks);
        return;
    }

    QJsonObject requestData;
    requestData["model"] = "deepseek-chat";
    requestData["messages"] = QJsonArray{
        QJsonObject{{"role", "system"},
                    {"content", "You are a task extractor. Extract actionable tasks from the email in JSON format."}},
        QJsonObject{{"role", "user"}, {"content", emailContent}}};

    makeRequest("https://api.deepseek.com/v1/chat/completions", QJsonDocument(requestData).toJson(),
                [this, cacheKey](const QString& response) {
                    m_cache->insert(cacheKey, new QString(response));
                    emit tasksExtracted(response);
                });
}

void LocalAIService::detectFollowUp(const QString& emailContent)
{
    QString cacheKey = QString("followup_%1").arg(emailContent.left(100));
    if (auto* cachedResult = m_cache->object(cacheKey)) {
        emit followUpDetected(*cachedResult);
        return;
    }

    QJsonObject requestData;
    requestData["model"] = "deepseek-chat";
    requestData["messages"] = QJsonArray{
        QJsonObject{{"role", "system"},
                    {"content", "You are a follow-up detector. Determine if this email needs a follow-up and when."}},
        QJsonObject{{"role", "user"}, {"content", emailContent}}};

    makeRequest("https://api.deepseek.com/v1/chat/completions", QJsonDocument(requestData).toJson(),
                [this, cacheKey](const QString& response) {
                    m_cache->insert(cacheKey, new QString(response));
                    emit followUpDetected(response);
                });
}

void LocalAIService::summarizeEmail(const QString& emailContent)
{
    QString cacheKey = QString("summary_%1").arg(emailContent.left(100));
    if (auto* cachedSummary = m_cache->object(cacheKey)) {
        emit summaryGenerated(*cachedSummary);
        return;
    }

    QJsonObject requestData;
    requestData["model"] = "deepseek-chat";
    requestData["messages"] =
        QJsonArray{QJsonObject{{"role", "system"},
                               {"content", "You are an email summarizer. Provide a concise summary of the email."}},
                   QJsonObject{{"role", "user"}, {"content", emailContent}}};

    makeRequest("https://api.deepseek.com/v1/chat/completions", QJsonDocument(requestData).toJson(),
                [this, cacheKey](const QString& response) {
                    m_cache->insert(cacheKey, new QString(response));
                    emit summaryGenerated(response);
                });
}

void LocalAIService::makeRequest(const QString& endpoint, const QByteArray& data,
                                 const std::function<void(const QString&)>& callback)
{
    if (!initialize()) {
        return;
    }

    QNetworkRequest request(QUrl(endpoint));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_apiKey).toUtf8());

    QNetworkReply* reply = m_networkManager->post(request, data);
    connect(reply, &QNetworkReply::finished, this, [this, reply, callback]() {
        reply->deleteLater();

        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument response = QJsonDocument::fromJson(reply->readAll());
            if (response.isObject() && response.object().contains("choices")) {
                QString content = response.object()["choices"]
                                      .toArray()
                                      .first()
                                      .toObject()["message"]
                                      .toObject()["content"]
                                      .toString();
                callback(content);
            } else {
                emit error(i18n("Invalid response from DeepSeek API"));
            }
        } else {
            handleError(reply);
        }
    });
}

void LocalAIService::handleError(QNetworkReply* reply)
{
    QString errorMessage = reply->errorString();
    emit error(i18n("API request failed: %1", errorMessage));

    // Store the failed request for retry
    m_pendingRetry = {reply->url(), reply->request().rawHeader("Authorization"),
                      reply->request().rawHeader("Content-Type"), reply->readAll()};

    // Start retry timer with exponential backoff
    static int retryCount = 0;
    if (retryCount < 3) {
        int delay = std::pow(2, retryCount) * 1000; // 1s, 2s, 4s
        m_retryTimer->start(delay);
        retryCount++;
    } else {
        retryCount = 0;
        m_pendingRetry = {};
    }
}

void LocalAIService::retryRequest()
{
    if (m_pendingRetry.url.isEmpty()) {
        return;
    }

    QNetworkRequest request(m_pendingRetry.url);
    request.setRawHeader("Authorization", m_pendingRetry.auth);
    request.setRawHeader("Content-Type", m_pendingRetry.contentType);

    QNetworkReply* reply = m_networkManager->post(request, m_pendingRetry.data);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() == QNetworkReply::NoError) {
            m_pendingRetry = {};
        } else {
            handleError(reply);
        }
    });
}

void LocalAIService::setCacheSize(int maxSize)
{
    m_cache->setMaxCost(maxSize);
}

void LocalAIService::clearCache()
{
    m_cache->clear();
}

} // namespace KMail
