/* SPDX-FileCopyrightText: 2010 Thomas McGuire <mcguire@kde.org>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#pragma once

#include "kmail_export.h"

#include <Akonadi/Collection>

#include <QHash>
#include <QModelIndex>
#include <QObject>

class QAction;

class KActionCollection;

namespace KMail {
class FolderShortcutCommand : public QObject
{
    Q_OBJECT

public:
    explicit FolderShortcutCommand(QWidget* mainwidget, const Akonadi::Collection& col);
    ~FolderShortcutCommand() override;

public Q_SLOTS:
    void start();
    /** Assign a QAction to the command which is used to trigger it. This
     * action will be deleted along with the command, so you don't need to
     * keep track of it separately. */
    void setAction(QAction*);

Q_SIGNALS:
    void selectCollectionFolder(const Akonadi::Collection& col);

private:
    const Akonadi::Collection mCollectionFolder;
    QWidget* const mMainWidget;
    QAction* mAction = nullptr;
};

class KMAIL_EXPORT FolderShortcutActionManager : public QObject
{
    Q_OBJECT

public:
    explicit FolderShortcutActionManager(QWidget* parent, KActionCollection* actionCollection);
    void createActions();

public Q_SLOTS:

    /**
     * Updates the shortcut action for this collection. Call this when a shortcut was
     * added, removed or changed.
     */
    void shortcutChanged(const Akonadi::Collection& collection);

private:
    /**
     * Removes the shortcut actions associated with a folder.
     */
    KMAIL_NO_EXPORT void slotCollectionRemoved(const Akonadi::Collection& collection);

    KMAIL_NO_EXPORT void slotRowsInserted(const QModelIndex& parent, int start, int end);

private:
    KMAIL_NO_EXPORT void updateShortcutsForIndex(const QModelIndex& parent, int start, int end);
    QHash<Akonadi::Collection::Id, FolderShortcutCommand*> mFolderShortcutCommands;
    KActionCollection* mActionCollection = nullptr;
    QWidget* mParent = nullptr;
};
} // namespace KMail
