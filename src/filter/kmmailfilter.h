#pragma once

#include <QObject>
#include <QString>

namespace KMail {

class LocalAIService;
class KMMessage;

class KMMailFilter : public QObject
{
    Q_OBJECT

public:
    explicit KMMailFilter(QObject* parent = nullptr);
    ~KMMailFilter() override;

    void processMessage(KMMessage* message);

Q_SIGNALS:
    void messageCategorized(KMMessage* message, const QString& category);
    void error(const QString& errorMessage);

private Q_SLOTS:
    void slotEmailCategorized(EmailCategory category);
    void slotAIError(const QString& error);

private:
    void initializeAIService();
    QString extractEmailContent(KMMessage* message);

    LocalAIService* m_aiService;
    KMMessage* m_currentMessage;
};
