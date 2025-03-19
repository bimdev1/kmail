/*
 * This file is part of KMail, the KDE mail client.
 * SPDX-FileCopyrightText: 2025 KMail AI Integration Team
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "aimanager.h"
#include "localaiservice.h"
#include "kmail_debug.h"

#include <KConfigGroup>
#include <KSharedConfig>

namespace KMail {

AIManager *AIManager::s_self = nullptr;

AIManager *AIManager::self()
{
    if (!s_self) {
        s_self = new AIManager();
    }
    return s_self;
}

void AIManager::destroy()
{
    delete s_self;
    s_self = nullptr;
}

AIManager::AIManager(QObject *parent)
    : QObject(parent)
    , m_enabled(false)
{
    // Read configuration
    KConfigGroup config(KSharedConfig::openConfig(), QStringLiteral("AI"));
    m_enabled = config.readEntry(QStringLiteral("Enabled"), false);

    // Initialize the AI service
    initialize();
}

AIManager::~AIManager()
{
}

bool AIManager::initialize()
{
    if (!m_enabled) {
        return false;
    }

    // Create the AI service
    m_service.reset(new LocalAIService(this));
    auto localService = qobject_cast<LocalAIService*>(m_service.get());
    if (localService) {
        bool success = localService->initialize();
        Q_EMIT initialized(success);
        return success;
    }

    Q_EMIT initialized(false);
    return false;
}

AIServiceInterface *AIManager::service() const
{
    return m_service.get();
}

bool AIManager::isEnabled() const
{
    return m_enabled;
}

void AIManager::setEnabled(bool enabled)
{
    if (m_enabled == enabled) {
        return;
    }

    m_enabled = enabled;

    // Save configuration
    KConfigGroup config(KSharedConfig::openConfig(), QStringLiteral("AI"));
    config.writeEntry(QStringLiteral("Enabled"), m_enabled);
    config.sync();

    if (m_enabled) {
        initialize();
    } else {
        m_service.reset();
    }

    Q_EMIT enabledChanged(m_enabled);
}

} // namespace KMail
