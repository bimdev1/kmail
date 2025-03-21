/*
   SPDX-FileCopyrightText: 2015-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kmailplugininterface.h"
#include "kmail_debug.h"
#include "kmmainwidget.h"

KMailPluginInterface::KMailPluginInterface(QObject* parent) : PimCommon::PluginInterface(parent)
{
    setPluginName(QStringLiteral("kmail"));
    setPluginDirectory(QStringLiteral("pim6/kmail/mainview"));
}

KMailPluginInterface::~KMailPluginInterface() = default;

KMailPluginInterface* KMailPluginInterface::self()
{
    static KMailPluginInterface s_self;
    return &s_self;
}

void KMailPluginInterface::setMainWidget(KMMainWidget* mainwindow)
{
    mMainWindow = mainwindow;
}

bool KMailPluginInterface::initializeInterfaceRequires(PimCommon::AbstractGenericPluginInterface* abstractInterface)
{
    if (!mMainWindow) {
        qCCritical(KMAIL_LOG) << "mainwindows not defined";
        return false;
    }
    auto interface = static_cast<PimCommon::GenericPluginInterface*>(abstractInterface);
    const PimCommon::GenericPluginInterface::RequireTypes requiresFeatures = interface->requiresFeatures();
    if (requiresFeatures & PimCommon::GenericPluginInterface::CurrentItems) {
        interface->setItems(mMainWindow->currentSelection());
    }
    if (requiresFeatures & PimCommon::GenericPluginInterface::Items) {
        qCDebug(KMAIL_LOG) << "PimCommon::GenericPluginInterface::Items not implemented";
    }
    if (requiresFeatures & PimCommon::GenericPluginInterface::CurrentCollection) {
        if (mMainWindow->currentCollection().isValid()) {
            interface->setCurrentCollection(mMainWindow->currentCollection());
        } else {
            qCDebug(KMAIL_LOG) << "Current Collection not defined";
        }
    }
    if (requiresFeatures & PimCommon::GenericPluginInterface::Collections) {
        qCDebug(KMAIL_LOG) << "PimCommon::GenericPluginInterface::Collection not implemented";
    }
    return true;
}

#include "moc_kmailplugininterface.cpp"
