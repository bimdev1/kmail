/*
    SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <KSieveCore/SieveImapInstanceInterfaceManager>
#include <KSieveUi/SieveDebugDialog>
#include "sieveimapinterface/kmailsieveimapinstanceinterface.h"
#include "sieveimapinterface/kmsieveimappasswordprovider.h"

#include <QApplication>
#include <QStandardPaths>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    QApplication::setApplicationName(QStringLiteral("sievedebugdialog"));
    QApplication::setApplicationVersion(QStringLiteral("1.0"));
    QStandardPaths::setTestModeEnabled(true);

    KSieveCore::SieveImapInstanceInterfaceManager::self()->setSieveImapInstanceInterface(
        new KMailSieveImapInstanceInterface);
    KMSieveImapPasswordProvider provider(nullptr);
    auto dlg = KSieveUi::SieveDebugDialog(&provider);
    dlg.exec();
    return 0;
}
