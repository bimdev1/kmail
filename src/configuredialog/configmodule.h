/*   -*- mode: C++; c-file-style: "gnu" -*-
 *   kmail: KDE mail client
 *   SPDX-FileCopyrightText: 2000 Espen Sand <espen@kde.org>
 *   SPDX-FileCopyrightText: 2001-2003 Marc Mutz <mutz@kde.org>
 *   Contains code segments and ideas from earlier kmail dialog code.
 *   SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#pragma once

#include <KCModule>
#include <KPluginMetaData>
#include <QObject>
class ConfigModule : public KCModule
{
public:
    explicit ConfigModule(QObject* parent, const KPluginMetaData& data) : KCModule(parent, data) {}

    ~ConfigModule() override = default;

    void defaults() override {}

    /** Should return the help anchor for this page or tab */
    virtual QString helpAnchor() const = 0;
};
