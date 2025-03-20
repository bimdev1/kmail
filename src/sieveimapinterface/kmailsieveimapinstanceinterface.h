/*
   SPDX-FileCopyrightText: 2017-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KSieveCore/SieveImapInstanceInterface>
#include "kmail_private_export.h"
class KMAILTESTS_TESTS_EXPORT KMailSieveImapInstanceInterface : public KSieveCore::SieveImapInstanceInterface
{
public:
    KMailSieveImapInstanceInterface();
    ~KMailSieveImapInstanceInterface() override = default;

    [[nodiscard]] QList<KSieveCore::SieveImapInstance> sieveImapInstances() override;

private:
    Q_DISABLE_COPY(KMailSieveImapInstanceInterface)
};
