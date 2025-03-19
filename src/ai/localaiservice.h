/*
 * This file is part of KMail, the KDE mail client.
 * SPDX-FileCopyrightText: 2025 KMail AI Integration Team
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

#include "aiserviceinterface.h"
#include <QObject>
#include <QString>
#include <QMap>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QCache>
#include <QCryptographicHash>

namespace KMail {

/**
 * @brief Local implementation of the AI service interface
 * 
 * This class implements the AI service interface using local Python scripts
 * for email categorization, reply drafting, task extraction, and follow-up reminders.
 */
class LocalAIService : public QObject, public AIServiceInterface
{
    Q_OBJECT

public:
    explicit LocalAIService(QObject *parent = nullptr);
    ~LocalAIService() override;

    /**
     * Initialize the AI service
     * @return true if initialization was successful, false otherwise
     */
    bool initialize() override;

    /**
     * Set the API key for DeepSeek integration
     * @param apiKey The API key to set
     */
    void setApiKey(const QString &apiKey);
    
    /**
     * Get the API key for DeepSeek integration
     * @return The API key
     */
    QString apiKey() const { return m_apiKey; }

    /**
     * Generate a reply to an email based on its content and user's writing style
     * @param emailContent The content of the email to reply to
     * @param prompt The prompt to use for generating the reply
     */
    void generateReply(const QString &emailContent, const QString &prompt) override;

    /**
     * Categorize an email based on its content
     * @param emailContent The content of the email to categorize
     */
    void categorizeEmail(const QString &emailContent) override;

    /**
     * Extract tasks from an email
     * @param emailContent The content of the email to extract tasks from
     */
    void extractTasks(const QString &emailContent) override;

    /**
     * Determine if an email needs a follow-up reminder
     * @param emailContent The content of the email to check
     */
    void detectFollowUp(const QString &emailContent) override;

    /**
     * Summarize an email
     * @param emailContent The content of the email to summarize
     */
    void summarizeEmail(const QString &emailContent) override;

    /**
     * Set the cache size
     * @param maxSize The maximum size of the cache
     */
    void setCacheSize(int maxSize);

    /**
     * Clear the cache
     */
    void clearCache();

Q_SIGNALS:
    /**
     * Emitted when an error occurs
     * @param message The error message
     */
    void error(const QString &message) override;

    /**
     * Emitted when a reply is generated
     * @param reply The generated reply
     */
    void replyGenerated(const QString &reply) override;

    /**
     * Emitted when tasks are extracted
     * @param tasks The extracted tasks
     */
    void tasksExtracted(const QStringList &tasks) override;

    /**
     * Emitted when a follow-up is detected
     * @param needsFollowUp True if the email needs a follow-up, false otherwise
     */
    void followUpDetected(bool needsFollowUp) override;

    /**
     * Emitted when an email is summarized
     * @param summary The summary of the email
     */
    void emailSummarized(const QString &summary) override;

    /**
     * Emitted when an email is categorized
     * @param category The category of the email
     */
    void emailCategorized(EmailCategory category) override;

private:
    struct PendingRequest {
        QString endpoint;
        QByteArray data;
        int retryCount{0};
        QTimer *retryTimer{nullptr};
    };

    struct CacheEntry {
        QString result;
        qint64 timestamp;
    };

    void makeRequest(const QString &endpoint, const QByteArray &data);
    void handleNetworkError(QNetworkReply *reply, const PendingRequest &request);
    void retryRequest(const PendingRequest &request);
    void cleanupRequest(const PendingRequest &request);
    
    QString getCachedResponse(const QString &key) const;
    void cacheResponse(const QString &key, const QString &response);
    QString generateCacheKey(const QString &endpoint, const QByteArray &data) const;

    static constexpr int MAX_RETRIES = 3;
    static constexpr int RETRY_DELAY_MS = 1000; // Start with 1 second
    static constexpr int DEFAULT_CACHE_SIZE = 1000;
    static constexpr qint64 CACHE_TTL_MS = 24 * 60 * 60 * 1000; // 24 hours
    
    QNetworkAccessManager *m_networkManager;
    QString m_apiKey;
    QMap<QNetworkReply*, PendingRequest> m_pendingRequests;
    QCache<QString, CacheEntry> m_cache;
    bool m_initialized;
    QMap<QString, EmailCategory> m_categoryMap;
}; // class LocalAIService

} // namespace KMail
