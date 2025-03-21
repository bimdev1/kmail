/*
   SPDX-FileCopyrightText: 2019-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QComboBox>
#include "kmail_private_export.h"

class KMAILTESTS_TESTS_EXPORT UndoSendCombobox : public QComboBox
{
    Q_OBJECT
public:
    explicit UndoSendCombobox(QWidget* parent = nullptr);
    ~UndoSendCombobox() override;

    [[nodiscard]] int delay() const;
    void setDelay(int val);

private:
    KMAIL_NO_EXPORT void initialize();
};
