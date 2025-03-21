/*
  SPDX-FileCopyrightText: 2015-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "potentialphishingemailwarningtest.h"
#include <QAction>
#include <QTest>
#include "../potentialphishingemailwarning.h"

PotentialPhishingEmailWarningTest::PotentialPhishingEmailWarningTest(QObject* parent) : QObject(parent) {}

PotentialPhishingEmailWarningTest::~PotentialPhishingEmailWarningTest() = default;

void PotentialPhishingEmailWarningTest::shouldHaveDefaultValue()
{
    PotentialPhishingEmailWarning w;
    QVERIFY(!w.isVisible());
    // Verify QVERIFY(w.isCloseButtonVisible());
    auto act = w.findChild<QAction*>(QStringLiteral("sendnow"));
    QVERIFY(act);
}

QTEST_MAIN(PotentialPhishingEmailWarningTest)

#include "moc_potentialphishingemailwarningtest.cpp"
