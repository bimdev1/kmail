/*
   SPDX-FileCopyrightText: 2013-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <Akonadi/Item>
#include <QObject>
#include <QQueue>
namespace Akonadi {
class AgentInstance;
class Collection;
} // namespace Akonadi

class FolderArchiveAccountInfo;
class FolderArchiveAgentJob;
class FolderArchiveCache;
class KJob;
class FolderArchiveManager : public QObject
{
    Q_OBJECT
public:
    explicit FolderArchiveManager(QObject* parent = nullptr);
    ~FolderArchiveManager() override;

    void load();
    void setArchiveItems(const Akonadi::Item::List& items, const QString& instanceName);
    void setArchiveItem(qlonglong itemId);

    void moveFailed(const QString& msg);
    void moveDone();

    [[nodiscard]] FolderArchiveCache* folderArchiveCache() const;
    void reloadConfig();

public Q_SLOTS:
    void slotCollectionRemoved(const Akonadi::Collection& collection);
    void slotInstanceRemoved(const Akonadi::AgentInstance& instance);

private:
    void slotFetchParentCollection(KJob* job);
    void slotFetchCollection(KJob* job);

    [[nodiscard]] FolderArchiveAccountInfo* infoFromInstanceName(const QString& instanceName) const;
    void nextJob();
    void removeInfo(const QString& instanceName);
    QQueue<FolderArchiveAgentJob*> mJobQueue;
    FolderArchiveAgentJob* mCurrentJob = nullptr;
    QList<FolderArchiveAccountInfo*> mListAccountInfo;
    FolderArchiveCache* const mFolderArchiveCache;
};
