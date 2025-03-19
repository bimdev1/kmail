/*
 * This file is part of KMail, the KDE mail client.
 * SPDX-FileCopyrightText: 2025 KMail AI Integration Team
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

class KActionCollection;

namespace KMail {

class KMMainWidget;

/**
 * Initialize the AI extension in KMMainWidget
 * @param mainWidget The KMMainWidget to initialize
 * @param actionCollection The action collection to add actions to
 */
void initializeAIExtension(KMMainWidget *mainWidget, KActionCollection *actionCollection);

/**
 * Clean up AI extension in KMMainWidget destructor
 * @param mainWidget The KMMainWidget to clean up
 */
void cleanupAIExtension(KMMainWidget *mainWidget);

} // namespace KMail
