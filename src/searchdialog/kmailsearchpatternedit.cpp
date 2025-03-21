/*
  SPDX-FileCopyrightText: 2013-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "kmailsearchpatternedit.h"

using namespace KMail;
KMailSearchPatternEdit::KMailSearchPatternEdit(QWidget* parent)
    : MailCommon::SearchPatternEdit(
          parent, static_cast<MailCommon::SearchPatternEdit::SearchPatternEditOptions>(SearchPatternEdit::NotShowTags),
          BalooMode)
{
}

KMailSearchPatternEdit::~KMailSearchPatternEdit() = default;

#include "moc_kmailsearchpatternedit.cpp"
