/*
   SPDX-FileCopyrightText: 2019-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "undosendcreatejobtest.h"
#include <QTest>
#include "undosend/undosendcreatejob.h"
QTEST_MAIN(UndoSendCreateJobTest)

UndoSendCreateJobTest::UndoSendCreateJobTest(QObject* parent) : QObject(parent) {}

void UndoSendCreateJobTest::shouldHaveDefaultValues()
{
    UndoSendCreateJob job;
    QCOMPARE(job.akonadiIndex(), -1);
    QCOMPARE(job.delay(), -1);
    QVERIFY(job.subject().isEmpty());
}

#include "moc_undosendcreatejobtest.cpp"
