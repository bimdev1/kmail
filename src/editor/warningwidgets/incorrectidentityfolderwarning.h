/*
   SPDX-FileCopyrightText: 2017-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KMessageWidget>
#include "kmail_private_export.h"

class KMAILTESTS_TESTS_EXPORT IncorrectIdentityFolderWarning : public KMessageWidget
{
    Q_OBJECT
public:
    explicit IncorrectIdentityFolderWarning(QWidget* parent = nullptr);
    ~IncorrectIdentityFolderWarning() override;

    void mailTransportIsInvalid();
    void fccIsInvalid();
    void identityInvalid();
    void dictionaryInvalid();
    void clearFccInvalid();

private:
    KMAIL_NO_EXPORT void addNewLine(QString& str);
    KMAIL_NO_EXPORT void updateText();
    KMAIL_NO_EXPORT void slotHideAnnimationFinished();
    bool mMailTransportIsInvalid = false;
    bool mFccIsInvalid = false;
    bool mIdentityIsInvalid = false;
    bool mDictionaryIsInvalid = false;
};
