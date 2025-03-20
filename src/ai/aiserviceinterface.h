/*
 * This file is part of KMail, the KDE mail client.
 * SPDX-FileCopyrightText: 2025 KMail AI Integration Team
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

#include <KMime/Message>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>

namespace KMail {

/**
 * @brief Interface for AI services in KMail
 *
 * This class defines the interface for all AI services used in KMail.
 * It provides methods for email categorization, reply drafting, task extraction,
 * and follow-up reminders.
 */
class AIServiceInterface : public QObject
{
    Q_OBJECT

public:
    /**
     * Email priority categories
     */
    enum EmailCategory { Urgent, FollowUp, LowPriority, Uncategorized };

    explicit AIServiceInterface(QObject* parent = nullptr);
    ~AIServiceInterface() override;

    /**
     * Categorize an email based on its content
     * @param emailContent The content of the email to categorize
     * @return The category of the email
     */
    virtual EmailCategory categorizeEmail(const QString& emailContent) = 0;

    /**
     * Generate a reply to an email based on its content and user's writing style
     * @param emailContent The content of the email to reply to
     * @param userHistory Previous emails sent by the user to learn their style
     * @return The generated reply
     */
    virtual QString generateReply(const QString& emailContent, const QStringList& userHistory) = 0;

    /**
     * Extract tasks from an email
     * @param emailContent The content of the email to extract tasks from
     * @return A list of tasks extracted from the email
     */
    virtual QStringList extractTasks(const QString& emailContent) = 0;

    /**
     * Determine if an email needs a follow-up reminder
     * @param emailContent The content of the email to check
     * @return true if the email needs a follow-up, false otherwise
     */
    virtual bool needsFollowUp(const QString& emailContent) = 0;

    /**
     * Suggest a follow-up date for an email
     * @param emailContent The content of the email
     * @return The suggested follow-up date as a string (YYYY-MM-DD)
     */
    virtual QString suggestFollowUpDate(const QString& emailContent) = 0;

    /**
     * Summarize an email
     * @param emailContent The content of the email to summarize
     * @return A summary of the email
     */
    virtual QString summarizeEmail(const QString& emailContent) = 0;

Q_SIGNALS:
    /**
     * Emitted when an email has been categorized
     * @param category The category of the email
     */
    void emailCategorized(EmailCategory category);

    /**
     * Emitted when a reply has been generated
     * @param reply The generated reply
     */
    void replyGenerated(const QString& reply);

    /**
     * Emitted when tasks have been extracted from an email
     * @param tasks The list of tasks extracted
     */
    void tasksExtracted(const QStringList& tasks);

    /**
     * Emitted when a follow-up reminder has been suggested
     * @param date The suggested follow-up date
     */
    void followUpSuggested(const QString& date);

    /**
     * Emitted when an email has been summarized
     * @param summary The summary of the email
     */
    void emailSummarized(const QString& summary);
};

} // namespace KMail
