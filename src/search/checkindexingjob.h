/*
   SPDX-FileCopyrightText: 2016-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <Akonadi/Collection>
#include <QObject>
namespace Akonadi {
namespace Search {
namespace PIM {
class IndexedItems;
}
} // namespace Search
} // namespace Akonadi
class KJob;
class CheckIndexingJob : public QObject
{
    Q_OBJECT
public:
    explicit CheckIndexingJob(Akonadi::Search::PIM::IndexedItems* indexedItems, QObject* parent = nullptr);
    ~CheckIndexingJob() override;

    void setCollection(const Akonadi::Collection& col);

    void start();

Q_SIGNALS:
    void finished(Akonadi::Collection::Id id, bool needToReindex);

private:
    void slotCollectionPropertiesFinished(KJob* job);
    void askForNextCheck(quint64 id, bool needToReindex = false);
    Akonadi::Collection mCollection;
    Akonadi::Search::PIM::IndexedItems* const mIndexedItems;
};
