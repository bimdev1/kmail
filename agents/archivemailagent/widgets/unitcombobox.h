/*
   SPDX-FileCopyrightText: 2015-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QComboBox>
#include "archivemailinfo.h"

class UnitComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit UnitComboBox(QWidget* parent = nullptr);
    ~UnitComboBox() override;

    [[nodiscard]] ArchiveMailInfo::ArchiveUnit unit() const;
    void setUnit(ArchiveMailInfo::ArchiveUnit unit);
};
