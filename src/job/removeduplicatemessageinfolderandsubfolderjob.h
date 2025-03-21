/*
  SPDX-FileCopyrightText: 2015-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include <Akonadi/Collection>
#include <QObject>
class KJob;
namespace KPIM {
class ProgressItem;
}
class RemoveDuplicateMessageInFolderAndSubFolderJob : public QObject
{
    Q_OBJECT
public:
    explicit RemoveDuplicateMessageInFolderAndSubFolderJob(QObject* parent = nullptr, QWidget* parentWidget = nullptr);
    ~RemoveDuplicateMessageInFolderAndSubFolderJob() override;

    void start();

    void setTopLevelCollection(const Akonadi::Collection& topLevelCollection);

private:
    void slotFetchCollectionFailed();
    void slotFetchCollectionDone(const Akonadi::Collection::List& list);
    void slotFinished(KJob* job);
    void slotRemoveDuplicatesUpdate(KJob* job, const QString& description);
    void slotRemoveDuplicatesCanceled(KPIM::ProgressItem* item);
    Akonadi::Collection mTopLevelCollection;
    QWidget* const mParentWidget;
};
