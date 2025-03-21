/*
   SPDX-FileCopyrightText: 2018 Daniel Vrátil <dvratil@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <MailCommon/MailInterfaces>
#include <QObject>

namespace Akonadi {
class EntityTreeModel;
class EntityMimeTypeFilterModel;
} // namespace Akonadi

namespace MailCommon {
class FolderCollectionMonitor;
}

class MailKernel : public QObject, public MailCommon::IKernel, public MailCommon::ISettings
{
    Q_OBJECT
public:
    explicit MailKernel(const KSharedConfigPtr& config, QObject* parent = nullptr);
    ~MailKernel() override;

    KIdentityManagementCore::IdentityManager* identityManager() override;
    MessageComposer::MessageSender* msgSender() override;

    Akonadi::EntityMimeTypeFilterModel* collectionModel() const override;
    KSharedConfig::Ptr config() override;
    void syncConfig() override;
    MailCommon::JobScheduler* jobScheduler() const override;
    Akonadi::ChangeRecorder* folderCollectionMonitor() const override;
    void updateSystemTray() override;

    [[nodiscard]] qreal closeToQuotaThreshold() override;
    [[nodiscard]] bool excludeImportantMailFromExpiry() override;
    [[nodiscard]] QStringList customTemplates() override;
    [[nodiscard]] Akonadi::Collection::Id lastSelectedFolder() override;
    void setLastSelectedFolder(Akonadi::Collection::Id col) override;
    [[nodiscard]] bool showPopupAfterDnD() override;
    void expunge(Akonadi::Collection::Id id, bool sync) override;

private:
    KSharedConfigPtr mConfig;
    KIdentityManagementCore::IdentityManager* const mIdentityManager;
    MessageComposer::MessageSender* const mMessageSender;
    MailCommon::FolderCollectionMonitor* mFolderCollectionMonitor = nullptr;
    Akonadi::EntityTreeModel* mEntityTreeModel = nullptr;
    Akonadi::EntityMimeTypeFilterModel* mCollectionModel = nullptr;
};
