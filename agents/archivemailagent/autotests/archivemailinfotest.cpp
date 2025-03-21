/*
   SPDX-FileCopyrightText: 2014-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "archivemailinfotest.h"
#include <Akonadi/Collection>
#include <KConfigGroup>
#include <KSharedConfig>
#include <QStandardPaths>
#include <QTest>
#include "../archivemailinfo.h"

ArchiveMailInfoTest::ArchiveMailInfoTest(QObject* parent) : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

ArchiveMailInfoTest::~ArchiveMailInfoTest() = default;

void ArchiveMailInfoTest::shouldHaveDefaultValue()
{
    ArchiveMailInfo info;
    QCOMPARE(info.saveCollectionId(), Akonadi::Collection::Id(-1));
    QCOMPARE(info.saveSubCollection(), false);
    QCOMPARE(info.url(), QUrl());
    QCOMPARE(info.archiveType(), MailCommon::BackupJob::Zip);
    QCOMPARE(info.archiveUnit(), ArchiveMailInfo::ArchiveDays);
    QCOMPARE(info.archiveAge(), 1);
    QCOMPARE(info.lastDateSaved(), QDate());
    QCOMPARE(info.maximumArchiveCount(), 0);
    QCOMPARE(info.isEnabled(), true);
    QCOMPARE(info.useRange(), false);
    QVERIFY(info.range().isEmpty());
}

void ArchiveMailInfoTest::shouldRestoreFromSettings()
{
    ArchiveMailInfo info;
    info.setSaveCollectionId(Akonadi::Collection::Id(42));
    info.setUrl(QUrl::fromLocalFile(QStringLiteral("/foo/foo")));
    info.setArchiveType(MailCommon::BackupJob::TarBz2);
    info.setArchiveUnit(ArchiveMailInfo::ArchiveMonths);
    info.setArchiveAge(5);
    info.setLastDateSaved(QDate::currentDate());
    info.setMaximumArchiveCount(5);
    info.setEnabled(false);
    info.setUseRange(true);
    info.setRange({8, 7});

    KConfigGroup grp(KSharedConfig::openConfig(), QStringLiteral("testsettings"));
    info.writeConfig(grp);

    const ArchiveMailInfo restoreInfo(grp);
    QCOMPARE(info, restoreInfo);
}

void ArchiveMailInfoTest::shouldCopyArchiveInfo()
{
    ArchiveMailInfo info;
    info.setSaveCollectionId(Akonadi::Collection::Id(42));
    info.setUrl(QUrl::fromLocalFile(QStringLiteral("/foo/foo")));
    info.setArchiveType(MailCommon::BackupJob::TarBz2);
    info.setArchiveUnit(ArchiveMailInfo::ArchiveMonths);
    info.setArchiveAge(5);
    info.setLastDateSaved(QDate::currentDate());
    info.setMaximumArchiveCount(5);
    info.setEnabled(false);
    info.setUseRange(true);
    info.setRange({8, 7});

    const ArchiveMailInfo copyInfo(info);
    QCOMPARE(info, copyInfo);
}

QTEST_MAIN(ArchiveMailInfoTest)

#include "moc_archivemailinfotest.cpp"
