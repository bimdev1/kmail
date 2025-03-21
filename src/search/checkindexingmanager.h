/*
   SPDX-FileCopyrightText: 2016-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <Akonadi/Collection>
#include <QAbstractItemModel>
#include <QObject>
namespace Akonadi {
namespace Search {
namespace PIM {
class IndexedItems;
}
} // namespace Search
} // namespace Akonadi
class QTimer;
class CheckIndexingManager : public QObject
{
    Q_OBJECT
public:
    explicit CheckIndexingManager(Akonadi::Search::PIM::IndexedItems* indexer, QObject* parent = nullptr);
    ~CheckIndexingManager() override;

    void start(QAbstractItemModel* collectionModel);

private:
    void checkNextCollection();

    void indexingFinished(qint64 index, bool reindexCollection);

    void initializeCollectionList(QAbstractItemModel* model, const QModelIndex& parentIndex = QModelIndex());
    void createJob();
    void callToReindexCollection();

    Akonadi::Search::PIM::IndexedItems* const mIndexedItems;
    Akonadi::Collection::List mListCollection;
    QTimer* const mTimer;
    QList<qint64> mCollectionsIndexed;
    QList<qint64> mCollectionsNeedToBeReIndexed;
    int mIndex = 0;
    bool mIsReady = true;
};
