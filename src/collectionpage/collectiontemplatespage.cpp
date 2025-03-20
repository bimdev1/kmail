/*
   SPDX-FileCopyrightText: 2009-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "collectiontemplatespage.h"

#include <MailCommon/CollectionTemplatesWidget>

#include <Akonadi/Collection>
#include <MailCommon/FolderSettings>
#include <MailCommon/MailKernel>
#include <TemplateParser/TemplatesConfiguration>
#include <templateparser/templatesconfiguration_kfg.h>

#include <KLocalizedString>

#include <QButtonGroup>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

using namespace Akonadi;
using namespace MailCommon;

CollectionTemplatesPage::CollectionTemplatesPage(QWidget* parent)
    : CollectionPropertiesPage(parent), mCollectionTemplateWidget(new MailCommon::CollectionTemplatesWidget(this))
{
    setObjectName(QLatin1String("KMail::CollectionTemplatesPage"));
    setPageTitle(i18nc("@title:tab Templates settings page.", "Templates"));
    init();
}

CollectionTemplatesPage::~CollectionTemplatesPage() = default;

bool CollectionTemplatesPage::canHandle(const Collection& collection) const
{
    return !CommonKernel->isSystemFolderCollection(collection) || CommonKernel->isMainFolderCollection(collection);
}

void CollectionTemplatesPage::init()
{
    auto topLayout = new QVBoxLayout(this);
    topLayout->setContentsMargins({});
    topLayout->addWidget(mCollectionTemplateWidget);
}

void CollectionTemplatesPage::load(const Collection& col)
{
    mCollectionTemplateWidget->load(col);
}

void CollectionTemplatesPage::save(Collection& col)
{
    mCollectionTemplateWidget->save(col);
}

#include "moc_collectiontemplatespage.cpp"
