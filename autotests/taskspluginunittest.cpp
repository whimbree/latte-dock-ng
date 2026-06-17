/*
    SPDX-FileCopyrightText: 2026 Ruizhi Zhong <ruizhi.zhong88@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "lattetasksplugin.h"
#include "contextmenuactionsbackend.h"

#include <QtQml/qqml.h>
#include <QSignalSpy>
#include <QTest>

class TasksPluginUnitTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void registersQmlTypes();
    void contextMenuBackendRejectsMissingParentAndInvalidLaunchers();
};

void TasksPluginUnitTest::registersQmlTypes()
{
    const char *uri = "org.kde.latte.private.tasks";
    LatteTasksPlugin plugin;
    plugin.registerTypes(uri);

    QVERIFY(qmlTypeId(uri, 0, 1, "ContextMenuActionsBackend") >= 0);
    QVERIFY(qmlTypeId(uri, 0, 1, "types") >= 0);
}

void TasksPluginUnitTest::contextMenuBackendRejectsMissingParentAndInvalidLaunchers()
{
    Latte::Tasks::ContextMenuActionsBackend backend;
    QObject parent;

    QVERIFY(backend.jumpListActions(QVariant(), nullptr).isEmpty());
    QVERIFY(backend.jumpListActions(QUrl(QStringLiteral("file:///tmp/missing.desktop")), &parent).isEmpty());
    QVERIFY(backend.placesActions(QVariant(), false, nullptr).isEmpty());
    QVERIFY(backend.placesActions(QUrl(QStringLiteral("file:///tmp/missing.desktop")), false, &parent).isEmpty());
    QVERIFY(backend.recentDocumentActions(QVariant(), nullptr).isEmpty());
    QVERIFY(backend.recentDocumentActions(QUrl(QStringLiteral("file:///tmp/missing.desktop")), &parent).isEmpty());

    QSignalSpy showAllSpy(&backend, &Latte::Tasks::ContextMenuActionsBackend::showAllPlaces);
    QCOMPARE(showAllSpy.count(), 0);
}

QTEST_GUILESS_MAIN(TasksPluginUnitTest)

#include "taskspluginunittest.moc"
