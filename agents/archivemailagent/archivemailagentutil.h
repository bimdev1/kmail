/*
   SPDX-FileCopyrightText: 2012-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QDate>
#include "archivemailinfo.h"

namespace ArchiveMailAgentUtil {
static QString archivePattern = QStringLiteral("ArchiveMailCollection %1");
[[nodiscard]] QDate diffDate(ArchiveMailInfo* info);
[[nodiscard]] bool needToArchive(ArchiveMailInfo* info);
[[nodiscard]] bool timeIsInRange(const QList<int>& range, QTime time);
} // namespace ArchiveMailAgentUtil
