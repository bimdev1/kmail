#pragma once

#include <QObject>
#include <QString>
#include <QMap>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QCache>
#include <functional>

namespace KMail {

/**
 * @brief Local implementation of the AI service using DeepSeek API
 * 
 * This class implements the AI service interface using the DeepSeek API
 * for email categorization, reply drafting, task extraction, and follow-up reminders.
 */
class LocalAIService : public QObject
{
    Q_OBJECT

public:
    explicit LocalAIService(QObject *parent = nullptr);
    ~LocalAIService() override;

    /**
     * Initialize the AI service
     * @return true if initialization was successful, false otherwise
     */
    bool initialize();

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
    void generateReply(const QString &emailContent, const QString &prompt);

    /**
     * Categorize an email based on its content
     * @param emailContent The content of the email to categorize
     */
    void categorizeEmail(const QString &emailContent);

    /**
     * Extract tasks from an email
     * @param emailContent The content of the email to extract tasks from
     */
    void extractTasks(const QString &emailContent);

    /**
     * Determine if an email needs a follow-up reminder
     * @param emailContent The content of the email to check
     */
    void detectFollowUp(const QString &emailContent);

    /**
     * Summarize an email
     * @param emailContent The content of the email to summarize
     */
    void summarizeEmail(const QString &emailContent);

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
    void error(const QString &message);

    /**
     * Emitted when a reply has been generated
     * @param reply The generated reply
     */
    void replyGenerated(const QString &reply);

    /**
     * Emitted when an email has been categorized
     * @param category The category assigned to the email
     */
    void emailCategorized(const QString &category);

    /**
     * Emitted when tasks have been extracted
     * @param tasks The extracted tasks
     */
    void tasksExtracted(const QString &tasks);

    /**
     * Emitted when follow-up status has been determined
     * @param needsFollowUp Whether the email needs a follow-up
     */
    void followUpDetected(const QString &followUp);

    /**
     * Emitted when a summary has been generated
     * @param summary The generated summary
     */
    void summaryGenerated(const QString &summary);

private:
    struct PendingRetry {
        QUrl url;
        QByteArray auth;
        QByteArray contentType;
        QByteArray data;
    };

    void makeRequest(const QString &endpoint, const QByteArray &data,
                    const std::function<void(const QString &)> &callback);
    void handleError(QNetworkReply *reply);
    void retryRequest();

    QString m_apiKey;
    QNetworkAccessManager *m_networkManager;
    QTimer *m_retryTimer;
    QCache<QString, QString> *m_cache;
    PendingRetry m_pendingRetry;
};

} // namespace KMail
