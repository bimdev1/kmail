/*
   SPDX-FileCopyrightText: 2011-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <Akonadi/Item>
#include <Akonadi/Tag>
#include <QDialog>
#include <QList>
#include "MailCommon/Tag"
#include "kmail_private_export.h"

class QListWidget;
class KActionCollection;
class KMAILTESTS_TESTS_EXPORT TagSelectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TagSelectDialog(QWidget* parent, int numberOfSelectedMessages, const Akonadi::Item& selectedItem);
    ~TagSelectDialog() override;
    [[nodiscard]] Akonadi::Tag::List selectedTag() const;

    void setActionCollection(const QList<KActionCollection*>& actionCollectionList);

private:
    KMAIL_NO_EXPORT void slotAddNewTag();
    KMAIL_NO_EXPORT void slotTagsFetched(KJob*);
    KMAIL_NO_EXPORT void writeConfig();
    KMAIL_NO_EXPORT void readConfig();
    KMAIL_NO_EXPORT void createTagList(bool updateList);
    enum ItemType { UrlTag = Qt::UserRole + 1 };
    const int mNumberOfSelectedMessages = -1;
    const Akonadi::Item mSelectedItem;

    Akonadi::Tag::List mCurrentSelectedTags;
    QList<MailCommon::Tag::Ptr> mTagList;
    QList<KActionCollection*> mActionCollectionList;
    QListWidget* const mListTag;
};
