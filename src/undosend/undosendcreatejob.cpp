/*
   SPDX-FileCopyrightText: 2019-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "undosendcreatejob.h"
#include "kmail_undo_send_debug.h"

#include <MessageComposer/SendLaterRemoveJob>

#include <KLocalizedString>
#include <KNotification>
#include <chrono>
#include <QTimer>
using namespace std::chrono_literals;

UndoSendCreateJob::UndoSendCreateJob(QObject* parent) : QObject(parent) {}

UndoSendCreateJob::~UndoSendCreateJob() = default;

bool UndoSendCreateJob::canStart() const
{
    if (mAkonadiIndex < 0 || mDelay <= 0) {
        return false;
    }
    return true;
}

bool UndoSendCreateJob::start()
{
    if (!canStart()) {
        qCWarning(KMAIL_UNDO_SEND_LOG) << "Impossible to start undosendcreatejob";
        deleteLater();
        return false;
    }
    mTimer = new QTimer(this);
    connect(mTimer, &QTimer::timeout, this, &UndoSendCreateJob::slotTimeOut);
    mTimer->setSingleShot(true);
    mTimer->start(mDelay * 1s);
    mNotification = new KNotification(QStringLiteral("undosend"), KNotification::Persistent, this);
    mNotification->setText(mSubject);

    auto undoSendAction = mNotification->addAction(i18n("Undo send"));
    connect(undoSendAction, &KNotificationAction::activated, this, &UndoSendCreateJob::undoSendEmail);

    connect(mNotification, &KNotification::closed, this, &UndoSendCreateJob::slotNotificationClosed);
    mNotification->sendEvent();

    return true;
}

void UndoSendCreateJob::slotTimeOut()
{
    qCDebug(KMAIL_UNDO_SEND_LOG) << "undo send timeout";
    mNotification->close();
    deleteLater();
}

void UndoSendCreateJob::slotNotificationClosed()
{
    qCDebug(KMAIL_UNDO_SEND_LOG) << "undo send slotNotificationClosed";
    mTimer->stop();
    deleteLater();
}

void UndoSendCreateJob::undoSendEmail()
{
    mTimer->stop();
    auto job = new MessageComposer::SendLaterRemoveJob(mAkonadiIndex, this);
    job->start();
}

QString UndoSendCreateJob::subject() const
{
    return mSubject;
}

void UndoSendCreateJob::setMessageInfoText(const QString& subject)
{
    mSubject = subject;
}

int UndoSendCreateJob::delay() const
{
    return mDelay;
}

void UndoSendCreateJob::setDelay(int delay)
{
    mDelay = delay;
}

qint64 UndoSendCreateJob::akonadiIndex() const
{
    return mAkonadiIndex;
}

void UndoSendCreateJob::setAkonadiIndex(qint64 akonadiIndex)
{
    mAkonadiIndex = akonadiIndex;
}

#include "moc_undosendcreatejob.cpp"
