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

class QNetworkAccessManager;

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
    ~LocalAIService() override = default;

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
    /**
     * Load the API key from storage
     */
    void loadApiKey();
    
    QNetworkAccessManager *m_networkManager;
    QString m_apiKey;
    bool m_initialized;
    QMap<QString, EmailCategory> m_categoryMap;
}; // class LocalAIService

} // namespace KMail
