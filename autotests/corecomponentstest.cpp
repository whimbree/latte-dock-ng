/*
    SPDX-FileCopyrightText: 2026 Ruizhi Zhong <ruizhi.zhong88@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "apptypes.h"
#include "plasma/extended/screenpool.h"
#include "primaryoutputwatcher.h"
#include "screenpool.h"

#include <KConfigGroup>
#include <KSharedConfig>

#include <QFile>
#include <QGuiApplication>
#include <QScreen>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

class TestPrimaryOutputWatcher : public Latte::PrimaryOutputWatcher
{
public:
    explicit TestPrimaryOutputWatcher(QObject *parent = nullptr)
        : Latte::PrimaryOutputWatcher(parent)
    {
    }

    using Latte::PrimaryOutputWatcher::setPrimaryOutputName;
};

class TestScreenPool : public Latte::ScreenPool
{
public:
    explicit TestScreenPool(KSharedConfig::Ptr config)
        : Latte::ScreenPool(config)
    {
    }

    using Latte::ScreenPool::firstAvailableId;
};

class CoreComponentsTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void appTypesMatchOnlyKnownLatteApplicationIds();
    void primaryOutputWatcherFindsScreensAndSignalsRealChanges();
    void screenPoolAllocatesStableIdsAndRejectsInvalidConnectors();
    void screenPoolLoadsStoredConnectorsAndIgnoresInvalidIds();
    void plasmaExtendedScreenPoolRejectsInvalidConnectors();
};

void CoreComponentsTest::appTypesMatchOnlyKnownLatteApplicationIds()
{
    QCOMPARE(Latte::App::preferredWaylandAppId(), QStringLiteral("latte-dock-ng"));
    QVERIFY(Latte::App::matchesSelfAppId(QStringLiteral("latte-dock-ng")));
    QVERIFY(Latte::App::matchesSelfAppId(QStringLiteral("latte-dock")));
    QVERIFY(Latte::App::matchesSelfAppId(QStringLiteral("org.kde.latte-dock")));
    QVERIFY(!Latte::App::matchesSelfAppId(QStringLiteral("org.kde.plasmashell")));
    QVERIFY(!Latte::App::matchesSelfAppId(QStringLiteral("")));
}

void CoreComponentsTest::primaryOutputWatcherFindsScreensAndSignalsRealChanges()
{
    TestPrimaryOutputWatcher watcher;
    QScreen *primary = qGuiApp->primaryScreen();
    QVERIFY(primary);

    QCOMPARE(watcher.screenForName(primary->name()), primary);
    QCOMPARE(watcher.screenForName(QStringLiteral("missing-output")), nullptr);

    QSignalSpy changedSpy(&watcher, &Latte::PrimaryOutputWatcher::primaryOutputNameChanged);
    watcher.setPrimaryOutputName(primary->name());
    QCOMPARE(changedSpy.count(), 0);

    watcher.setPrimaryOutputName(QStringLiteral("missing-output"));
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(changedSpy.first().at(1).toString(), QStringLiteral("missing-output"));
    QCOMPARE(watcher.primaryScreen(), primary);
}

void CoreComponentsTest::screenPoolAllocatesStableIdsAndRejectsInvalidConnectors()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString configPath = dir.path() + QStringLiteral("/lattedockrc");
    KSharedConfig::Ptr config = KSharedConfig::openConfig(configPath);

    TestScreenPool pool(config);
    QCOMPARE(pool.firstAvailableId(), Latte::ScreenPool::FIRSTSCREENID);
    QCOMPARE(pool.id(QStringLiteral("HDMI-A-1")), Latte::ScreenPool::NOSCREENID);

    pool.insertScreenMapping(QStringLiteral(":0.0"));
    QCOMPARE(pool.screensTable().rowCount(), 0);

    pool.insertScreenMapping(QStringLiteral("HDMI-A-1"));
    QCOMPARE(pool.id(QStringLiteral("HDMI-A-1")), Latte::ScreenPool::FIRSTSCREENID);
    QCOMPARE(pool.connector(Latte::ScreenPool::FIRSTSCREENID), QStringLiteral("HDMI-A-1"));
    QCOMPARE(pool.firstAvailableId(), Latte::ScreenPool::FIRSTSCREENID + 1);

    pool.insertScreenMapping(QStringLiteral("HDMI-A-1"));
    QCOMPARE(pool.screensTable().rowCount(), 1);
}

void CoreComponentsTest::screenPoolLoadsStoredConnectorsAndIgnoresInvalidIds()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString configPath = dir.path() + QStringLiteral("/lattedockrc");
    KSharedConfig::Ptr config = KSharedConfig::openConfig(configPath);
    KConfigGroup group(config, QStringLiteral("ScreenConnectors"));
    Latte::Data::Screen hdmi;
    hdmi.name = QStringLiteral("HDMI-A-1");
    hdmi.geometry = QRect(0, 0, 1920, 1080);
    Latte::Data::Screen displayPort;
    displayPort.name = QStringLiteral("DP-1");
    displayPort.geometry = QRect(1920, 0, 1920, 1080);

    group.writeEntry("0", QStringLiteral("ignored-primary"));
    group.writeEntry("-1", QStringLiteral("ignored-negative"));
    group.writeEntry("10", hdmi.serialize());
    group.writeEntry("11", displayPort.serialize());
    group.sync();

    TestScreenPool pool(config);
    pool.load();

    QCOMPARE(pool.id(QStringLiteral("HDMI-A-1")), 10);
    QCOMPARE(pool.id(QStringLiteral("DP-1")), 11);
    QCOMPARE(pool.connector(10), QStringLiteral("HDMI-A-1"));
    QCOMPARE(pool.connector(11), QStringLiteral("DP-1"));
    QVERIFY(!pool.hasScreenId(0));
    QVERIFY(!pool.hasScreenId(-1));
    QCOMPARE(pool.firstAvailableId(), 12);
}

void CoreComponentsTest::plasmaExtendedScreenPoolRejectsInvalidConnectors()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    qputenv("XDG_CONFIG_HOME", dir.path().toUtf8());

    Latte::PlasmaExtended::ScreenPool pool;
    QVERIFY(QMetaObject::invokeMethod(&pool, "insertScreenMapping",
                                      Q_ARG(int, 1),
                                      Q_ARG(QString, QString())));
    QVERIFY(QMetaObject::invokeMethod(&pool, "insertScreenMapping",
                                      Q_ARG(int, 2),
                                      Q_ARG(QString, QStringLiteral(":0.0"))));
    QVERIFY(QMetaObject::invokeMethod(&pool, "insertScreenMapping",
                                      Q_ARG(int, 3),
                                      Q_ARG(QString, QStringLiteral("HDMI-A-1"))));

    QCOMPARE(pool.connector(1), QString());
    QCOMPARE(pool.connector(2), QString());
    QCOMPARE(pool.connector(3), QStringLiteral("HDMI-A-1"));
    QCOMPARE(pool.id(QStringLiteral("HDMI-A-1")), 3);
}

QTEST_MAIN(CoreComponentsTest)

#include "corecomponentstest.moc"
