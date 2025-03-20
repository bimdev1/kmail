/*
  SPDX-FileCopyrightText: 2021-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/
#include <KPluginFactory>
#include "kcm_kmail.cpp"

K_PLUGIN_CLASS_WITH_JSON(AccountsPage, "kmail_config_accounts.json")

#include "kcm_kmail_accounts.moc"
