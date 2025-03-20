#include "kmmainwidgetaifilter.h"
#include "../filter/kmmailfilter.h"
#include "../kmmainwidget.h"
#include "../kmmessage.h"

#include <KLocalizedString>
#include <KMessageBox>

namespace KMail {

KMMainWidgetAIFilter::KMMainWidgetAIFilter(KMMainWidget* parent)
    : QObject(parent), m_mainWidget(parent), m_mailFilter(new KMMailFilter(this))
{
    connect(m_mailFilter, &KMMailFilter::messageCategorized, this, &KMMainWidgetAIFilter::slotMessageCategorized);
    connect(m_mailFilter, &KMMailFilter::error, this, &KMMainWidgetAIFilter::slotFilterError);
}

KMMainWidgetAIFilter::~KMMainWidgetAIFilter() {}

void KMMainWidgetAIFilter::processNewMessage(KMMessage* message)
{
    if (message) {
        m_mailFilter->processMessage(message);
    }
}

void KMMainWidgetAIFilter::slotMessageCategorized(KMMessage* message, const QString& category)
{
    if (!message) {
        return;
    }

    // Update the message view to show the new category
    message->setCategory(category);
    message->save();

    // Notify the user about the categorization
    QString notification = i18n("Message \"%1\" categorized as %2", message->subject().trimmed(), category);

    m_mainWidget->showStatusMessage(notification);
}

void KMMainWidgetAIFilter::slotFilterError(const QString& error)
{
    KMessageBox::error(m_mainWidget, error, i18n("AI Filter Error"));
}

} // namespace KMail
