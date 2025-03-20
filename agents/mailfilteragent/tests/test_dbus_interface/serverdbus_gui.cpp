/*
   SPDX-FileCopyrightText: 2023-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QApplication>
#include <QCommandLineParser>
#include <QStandardPaths>
#include "serverdbuswidget.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(app);

    qRegisterMetaType<QList<qint64>>("QList<qint64>");
    auto serverDbusWidget = new ServerDbusWidget(nullptr);
    serverDbusWidget->show();
    return app.exec();
}
