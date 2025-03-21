/*
   SPDX-FileCopyrightText: 2019-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>
#include "kmail_private_export.h"

class KMAILTESTS_TESTS_EXPORT UndoSendManager : public QObject
{
    Q_OBJECT
public:
    struct KMAILTESTS_TESTS_EXPORT UndoSendManagerInfo {
        QString subject;
        QString to;
        qint64 index = -1;
        int delay = -1;
        [[nodiscard]] QString generateMessageInfoText() const;
        [[nodiscard]] bool isValid() const;
    };
    explicit UndoSendManager(QObject* parent = nullptr);
    ~UndoSendManager() override;
    static UndoSendManager* self();

    void addItem(const UndoSendManagerInfo& info);
};
Q_DECLARE_TYPEINFO(UndoSendManager::UndoSendManagerInfo, Q_RELOCATABLE_TYPE);
