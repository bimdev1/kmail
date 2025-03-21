/*
    SPDX-FileCopyrightText: 2005 Till Adam <adam@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "util.h"

#include <MessageComposer/MessageHelper>
#include <MessageCore/StringUtil>
#include <PimCommon/PimUtil>
#include "job/handleclickedurljob.h"

#include <KLocalizedString>
#include <KMessageBox>
#include <KMime/Message>
#include "kmail_debug.h"

#include <QAction>
#include <QProcess>
#include <QStandardPaths>

using namespace MailCommon;
using namespace KMail;
using namespace Qt::Literals::StringLiterals;

KMime::Types::Mailbox::List KMail::Util::mailingListsFromMessage(const Akonadi::Item& item)
{
    KMime::Types::Mailbox::List addresses;
    // determine the mailing list posting address
    const Akonadi::Collection parentCollection = item.parentCollection();
    if (parentCollection.isValid()) {
        const QSharedPointer<FolderSettings> fd = FolderSettings::forCollection(parentCollection, false);
        if (fd->isMailingListEnabled() && !fd->mailingListPostAddress().isEmpty()) {
            KMime::Types::Mailbox mailbox;
            mailbox.fromUnicodeString(fd->mailingListPostAddress());
            addresses << mailbox;
        }
    }

    return addresses;
}

Akonadi::Item::Id KMail::Util::putRepliesInSameFolder(const Akonadi::Item& item)
{
    const Akonadi::Collection parentCollection = item.parentCollection();
    if (parentCollection.isValid()) {
        const QSharedPointer<FolderSettings> fd = FolderSettings::forCollection(parentCollection, false);
        if (fd->putRepliesInSameFolder()) {
            return parentCollection.id();
        }
    }
    return -1;
}

bool KMail::Util::handleClickedURL(const QUrl& url, const QSharedPointer<MailCommon::FolderSettings>& folder,
                                   const Akonadi::Collection& collection, QWidget* parentWidget)
{
    if (url.scheme() == "mailto"_L1) {
        auto job = new HandleClickedUrlJob;
        job->setUrl(url);
        job->setFolder(folder);
        job->setCurrentCollection(collection);
        job->start();
        return true;
    } else if (url.scheme() == "certificate"_L1) {
        QStringList lst;
        if (parentWidget) {
            lst << QStringLiteral("--parent-windowid")
                << QString::number(static_cast<qlonglong>(parentWidget->winId()));
        }
        lst << QStringLiteral("--query") << url.path();
        QString exec = PimCommon::Util::findExecutable(QStringLiteral("kleopatra"));
        if (exec.isEmpty()) {
            qCWarning(KMAIL_LOG) << "Could not find kleopatra executable in PATH";
            return false;
        }
        QProcess::startDetached(exec, lst);
        return true;
    }
    qCWarning(KMAIL_LOG) << "Can't handle URL:" << url;
    return false;
}

bool KMail::Util::mailingListsHandleURL(const QList<QUrl>& lst,
                                        const QSharedPointer<MailCommon::FolderSettings>& folder,
                                        const Akonadi::Collection& collection)
{
    const QString handler =
        (folder->mailingList().handler() == MailingList::KMail) ? QStringLiteral("mailto") : QStringLiteral("https");

    QUrl urlToHandle;
    QList<QUrl>::ConstIterator end(lst.constEnd());
    for (QList<QUrl>::ConstIterator itr = lst.constBegin(); itr != end; ++itr) {
        if (handler == (*itr).scheme()) {
            urlToHandle = *itr;
            break;
        }
    }
    if (urlToHandle.isEmpty() && !lst.empty()) {
        urlToHandle = lst.constFirst();
    }

    if (!urlToHandle.isEmpty()) {
        return Util::handleClickedURL(urlToHandle, folder, collection);
    } else {
        qCWarning(KMAIL_LOG) << "Can't handle url";
        return false;
    }
}

bool KMail::Util::mailingListPost(const QSharedPointer<MailCommon::FolderSettings>& fd, const Akonadi::Collection& col)
{
    if (fd) {
        return KMail::Util::mailingListsHandleURL(fd->mailingList().postUrls(), fd, col);
    }
    return false;
}

bool KMail::Util::mailingListSubscribe(const QSharedPointer<MailCommon::FolderSettings>& fd,
                                       const Akonadi::Collection& col)
{
    if (fd) {
        return KMail::Util::mailingListsHandleURL(fd->mailingList().subscribeUrls(), fd, col);
    }
    return false;
}

bool KMail::Util::mailingListUnsubscribe(const QSharedPointer<MailCommon::FolderSettings>& fd,
                                         const Akonadi::Collection& col)
{
    if (fd) {
        return KMail::Util::mailingListsHandleURL(fd->mailingList().unsubscribeUrls(), fd, col);
    }
    return false;
}

bool KMail::Util::mailingListArchives(const QSharedPointer<MailCommon::FolderSettings>& fd,
                                      const Akonadi::Collection& col)
{
    if (fd) {
        return KMail::Util::mailingListsHandleURL(fd->mailingList().archiveUrls(), fd, col);
    }
    return false;
}

bool KMail::Util::mailingListHelp(const QSharedPointer<MailCommon::FolderSettings>& fd, const Akonadi::Collection& col)
{
    if (fd) {
        return KMail::Util::mailingListsHandleURL(fd->mailingList().helpUrls(), fd, col);
    }
    return false;
}

void KMail::Util::lastEncryptAndSignState(bool& lastEncrypt, bool& lastSign, const KMime::Message::Ptr& msg)
{
    lastSign = KMime::isSigned(msg.data());
    lastEncrypt = KMime::isEncrypted(msg.data());
}

void KMail::Util::addQActionHelpText(QAction* action, const QString& text)
{
    action->setStatusTip(text);
    action->setToolTip(text);
    if (action->whatsThis().isEmpty()) {
        action->setWhatsThis(text);
    }
}

void KMail::Util::setActionTrashOrDelete(QAction* action, bool isInTrashFolder)
{
    if (action) {
        action->setText(isInTrashFolder ? i18nc("@action Hard delete, bypassing trash", "&Delete")
                                        : i18n("&Move to Trash"));
        action->setIcon(isInTrashFolder ? QIcon::fromTheme(QStringLiteral("edit-delete-shred"))
                                        : QIcon::fromTheme(QStringLiteral("edit-delete")));
        // Use same text as in Text property. Change it in kf5
        action->setToolTip(isInTrashFolder ? i18nc("@action Hard delete, bypassing trash", "Delete")
                                           : i18n("Move to Trash"));
    }
}

void KMail::Util::executeAccountWizard(QWidget* parentWidget)
{
    const QString path = PimCommon::Util::findExecutable(QStringLiteral("accountwizard"));
    if (path.isEmpty() || !QProcess::startDetached(path, {})) {
        KMessageBox::error(parentWidget,
                           i18n("Could not start the account wizard. "
                                "Please make sure you have AccountWizard properly installed."),
                           i18nc("@title:window", "Unable to start account wizard"));
    }
}
