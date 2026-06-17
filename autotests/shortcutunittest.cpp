/*
    SPDX-FileCopyrightText: 2026 Ruizhi Zhong <ruizhi.zhong88@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "modifiertracker.h"
#include "shortcutstracker.h"

#include <KConfigGroup>
#include <KSharedConfig>

#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

class ShortcutsUnitTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void modifierTrackerStartsWithNoPressedModifiers();
    void shortcutsTrackerParsesActivateBadgesFromConfig();
};

void ShortcutsUnitTest::modifierTrackerStartsWithNoPressedModifiers()
{
    Latte::ShortcutsPart::ModifierTracker tracker(this);

    QVERIFY(tracker.noModifierPressed());
    QVERIFY(!tracker.sequenceModifierPressed(QKeySequence()));
    QVERIFY(!tracker.sequenceModifierPressed(QKeySequence(QKeyCombination(Qt::ControlModifier, Qt::Key_1))));
    QVERIFY(!tracker.singleModifierPressed(Qt::Key_Control));

    tracker.blockModifierTracking(Qt::Key_Control);
    tracker.blockModifierTracking(Qt::Key_Control);
    tracker.unblockModifierTracking(Qt::Key_Control);
    QVERIFY(tracker.noModifierPressed());
}

void ShortcutsUnitTest::shortcutsTrackerParsesActivateBadgesFromConfig()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    qputenv("XDG_CONFIG_HOME", dir.path().toUtf8());

    const QString configPath = dir.path() + QStringLiteral("/kglobalshortcutsrc");
    KSharedConfig::Ptr config = KSharedConfig::openConfig(configPath);
    KConfigGroup latteGroup(config, QStringLiteral("lattedock"));
    latteGroup.writeEntry(QStringLiteral("activate entry 1"), QStringList{QStringLiteral("Meta+1")});
    latteGroup.writeEntry(QStringLiteral("activate entry 2"), QStringList{QStringLiteral("Meta+A")});
    latteGroup.writeEntry(QStringLiteral("activate entry 3"), QStringList{QStringLiteral("Ctrl+Alt+B")});
    latteGroup.sync();
    config->sync();

    Latte::ShortcutsPart::ShortcutsTracker tracker(this);
    QSignalSpy changedSpy(&tracker, &Latte::ShortcutsPart::ShortcutsTracker::badgesForActivateChanged);

    QCOMPARE(tracker.badgesForActivate().count(), 19);
    QCOMPARE(tracker.badgesForActivate().at(0), QStringLiteral("1"));
    QCOMPARE(tracker.badgesForActivate().at(1), QStringLiteral("a"));
    QCOMPARE(tracker.badgesForActivate().at(2), QStringLiteral("B"));
    QVERIFY(tracker.basedOnPositionEnabled());
    QVERIFY(tracker.appletsWithPlasmaShortcuts().isEmpty());
    QCOMPARE(tracker.appletShortcutBadge(123), QString());
    QCOMPARE(changedSpy.count(), 0);
}

QTEST_GUILESS_MAIN(ShortcutsUnitTest)

#include "shortcutunittest.moc"
