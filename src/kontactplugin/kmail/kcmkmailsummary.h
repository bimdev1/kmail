/*
  This file is part of Kontact.

  SPDX-FileCopyrightText: 2004 Tobias Koenig <tokoe@kde.org>
  SPDX-FileCopyrightText: 2013-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later WITH Qt-Commercial-exception-1.0
*/

#pragma once
#include <KCModule>
#include <KViewStateMaintainer>
namespace Akonadi {
class ETMViewStateSaver;
}

class QCheckBox;

namespace PimCommon {
class CheckedCollectionWidget;
}

class KCMKMailSummary : public KCModule
{
    Q_OBJECT

public:
    explicit KCMKMailSummary(QObject* parent, const KPluginMetaData& data);

    void load() override;
    void save() override;
    void defaults() override;

private:
    void modified();
    void initGUI();
    void initFolders();
    void loadFolders();
    void storeFolders();

    PimCommon::CheckedCollectionWidget* const mCheckedCollectionWidget;
    QCheckBox* const mFullPath;
    KViewStateMaintainer<Akonadi::ETMViewStateSaver>* mModelState = nullptr;
};
