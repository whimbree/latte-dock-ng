/*
    SPDX-FileCopyrightText: 2026 Ruizhi Zhong <ruizhi.zhong88@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "panelbackground.h"

#include <QSignalSpy>
#include <QTest>

class PlasmaExtendedUnitTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void panelBackgroundStartsWithNeutralMetrics();
};

void PlasmaExtendedUnitTest::panelBackgroundStartsWithNeutralMetrics()
{
    Latte::PlasmaExtended::PanelBackground top(Plasma::Types::TopEdge, nullptr);
    Latte::PlasmaExtended::PanelBackground right(Plasma::Types::RightEdge, nullptr);

    QCOMPARE(top.paddingTop(), 0);
    QCOMPARE(top.paddingLeft(), 0);
    QCOMPARE(top.paddingBottom(), 0);
    QCOMPARE(top.paddingRight(), 0);
    QCOMPARE(top.shadowSize(), 0);
    QCOMPARE(top.roundness(), 0);
    QCOMPARE(top.maxOpacity(), 1.0f);
    QCOMPARE(top.shadowColor(), QColor());

    QCOMPARE(right.paddingTop(), 0);
    QCOMPARE(right.paddingLeft(), 0);
    QCOMPARE(right.paddingBottom(), 0);
    QCOMPARE(right.paddingRight(), 0);
    QCOMPARE(right.shadowSize(), 0);
    QCOMPARE(right.roundness(), 0);
    QCOMPARE(right.maxOpacity(), 1.0f);
    QCOMPARE(right.shadowColor(), QColor());
}

QTEST_GUILESS_MAIN(PlasmaExtendedUnitTest)

#include "plasmaextendedunittest.moc"
