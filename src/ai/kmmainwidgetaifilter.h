#pragma once

#include <QObject>

namespace KMail {

class KMMainWidget;
class KMMailFilter;
class KMMessage;

class KMMainWidgetAIFilter : public QObject
{
    Q_OBJECT

public:
    explicit KMMainWidgetAIFilter(KMMainWidget *parent);
    ~KMMainWidgetAIFilter() override;

    void processNewMessage(KMMessage *message);

private Q_SLOTS:
    void slotMessageCategorized(KMMessage *message, const QString &category);
    void slotFilterError(const QString &error);

private:
    KMMainWidget *m_mainWidget;
    KMMailFilter *m_mailFilter;
};
