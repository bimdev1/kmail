/*
   SPDX-FileCopyrightText: 2020-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include <KUserFeedback/AbstractDataSource>
#include "kmail_private_export.h"

class KMAILTESTS_TESTS_EXPORT PluginInfoSource : public KUserFeedback::AbstractDataSource
{
public:
    PluginInfoSource();

    [[nodiscard]] QString name() const override;
    [[nodiscard]] QString description() const override;

    [[nodiscard]] QVariant data() override;
};
