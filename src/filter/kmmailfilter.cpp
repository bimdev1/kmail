#include "kmmailfilter.h"
#include "../ai/localaiservice.h"
#include "../kmfolder.h"
#include "../kmkernel.h"
#include "../kmmessage.h"

#include <KLocalizedString>

namespace KMail {

KMMailFilter::KMMailFilter(QObject* parent) : QObject(parent), m_aiService(nullptr), m_currentMessage(nullptr)
{
    initializeAIService();
}

KMMailFilter::~KMMailFilter()
{
    delete m_aiService;
}

void KMMailFilter::initializeAIService()
{
    if (!m_aiService) {
        m_aiService = new LocalAIService(this);
        connect(m_aiService, &LocalAIService::emailCategorized, this, &KMMailFilter::slotEmailCategorized);
        connect(m_aiService, &LocalAIService::error, this, &KMMailFilter::slotAIError);
    }
}

void KMMailFilter::processMessage(KMMessage* message)
{
    if (!message) {
        return;
    }

    m_currentMessage = message;
    QString content = extractEmailContent(message);
    m_aiService->categorizeEmail(content);
}

QString KMMailFilter::extractEmailContent(KMMessage* message)
{
    if (!message) {
        return QString();
    }

    QString content;
    content += QStringLiteral("Subject: ") + message->subject() + QStringLiteral("\n\n");
    content += message->plainTextContent();
    return content;
}

void KMMailFilter::slotEmailCategorized(EmailCategory category)
{
    if (!m_currentMessage) {
        return;
    }

    QString categoryStr;
    switch (category) {
    case EmailCategory::Urgent:
        categoryStr = QStringLiteral("Urgent");
        m_currentMessage->setLabel(QStringLiteral("urgent"));
        break;
    case EmailCategory::Normal:
        categoryStr = QStringLiteral("Normal");
        m_currentMessage->setLabel(QStringLiteral("normal"));
        break;
    case EmailCategory::Low:
        categoryStr = QStringLiteral("Low Priority");
        m_currentMessage->setLabel(QStringLiteral("low"));
        break;
    default:
        categoryStr = QStringLiteral("Uncategorized");
        break;
    }

    emit messageCategorized(m_currentMessage, categoryStr);
    m_currentMessage = nullptr;
}

void KMMailFilter::slotAIError(const QString& error)
{
    emit this->error(i18n("AI categorization error: %1", error));
    m_currentMessage = nullptr;
}

} // namespace KMail
