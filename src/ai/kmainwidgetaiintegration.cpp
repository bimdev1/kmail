/*
 * This file is part of KMail, the KDE mail client.
 * SPDX-FileCopyrightText: 2025 KMail AI Integration Team
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "kmainwidgetaiintegration.h"
#include "../kmmainwidget.h"
#include "aimainwidgetextension.h"
#include "aimanager.h"
#include "kmail_debug.h"

namespace KMail {

// This file contains the implementation of AI-related methods for KMMainWidget
// These methods are declared in kmmainwidget.h but implemented here to keep the
// main file size manageable and to separate AI functionality.

// Initialize the AI extension in KMMainWidget constructor
void initializeAIExtension(KMMainWidget *mainWidget, KActionCollection *actionCollection)
{
    // Create the AI extension
    mainWidget->m_aiExtension = new AIMainWidgetExtension(mainWidget, actionCollection);
    
    // Initialize the extension
    mainWidget->m_aiExtension->initialize();
    
    // Connect signals/slots
    QObject::connect(mainWidget, &KMMainWidget::selectionChanged, 
                    mainWidget, &KMMainWidget::slotUpdateAIActions);
}

// Clean up AI extension in KMMainWidget destructor
void cleanupAIExtension(KMMainWidget *mainWidget)
{
    // The extension will be deleted automatically as a child of KMMainWidget
    mainWidget->m_aiExtension = nullptr;
}

} // namespace KMail
