/*
   SPDX-FileCopyrightText: 2014-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KActionMenu>
#include <MessageViewer/Viewer>
#include "kmail_private_export.h"

class KMAILTESTS_TESTS_EXPORT DisplayMessageFormatActionMenu : public KActionMenu
{
    Q_OBJECT
public:
    explicit DisplayMessageFormatActionMenu(QObject* parent = nullptr);
    ~DisplayMessageFormatActionMenu() override;

    [[nodiscard]] MessageViewer::Viewer::DisplayFormatMessage displayMessageFormat() const;
    void setDisplayMessageFormat(MessageViewer::Viewer::DisplayFormatMessage displayMessageFormat);

Q_SIGNALS:
    void changeDisplayMessageFormat(MessageViewer::Viewer::DisplayFormatMessage format);

private:
    KMAIL_NO_EXPORT void slotChangeDisplayMessageFormat(QAction* act);
    KMAIL_NO_EXPORT void updateMenu();
    MessageViewer::Viewer::DisplayFormatMessage mDisplayMessageFormat = MessageViewer::Viewer::UseGlobalSetting;
};
