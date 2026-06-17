/*
    SPDX-FileCopyrightText: 2026 Ruizhi Zhong <ruizhi.zhong88@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "lattecontainmentplugin.h"

#include <QtQml/qqml.h>
#include <QTest>

class PluginRegistrationUnitTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void containmentPluginRegistersTypes();
};

void PluginRegistrationUnitTest::containmentPluginRegistersTypes()
{
    const char *uri = "org.kde.latte.private.containment";
    LatteContainmentPlugin plugin;
    plugin.registerTypes(uri);

    QVERIFY(qmlTypeId(uri, 0, 1, "LayoutManager") >= 0);
    QVERIFY(qmlTypeId(uri, 0, 1, "types") >= 0);
}

QTEST_GUILESS_MAIN(PluginRegistrationUnitTest)

#include "pluginregistrationunittest.moc"
