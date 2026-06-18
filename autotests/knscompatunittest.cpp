/*
    SPDX-FileCopyrightText: 2026 Ruizhi Zhong <ruizhi.zhong88@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "knscompat.h"

#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QTest>

class KnsCompatUnitTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init();
    void cleanup();
    void latestStampStillRepairsMissingControlsOverride();
    void firstRunWritesPatchedOverridesAndStamp();
    void optOutDoesNotWriteOverrides();
    void usesConfiguredSystemQmlRoot();
    void usesConfiguredUserQmlRoot();
    void missingSystemQmlRootDoesNotCreatePartialOverrides();

private:
    void isolateHomeAndData(QTemporaryDir &home, QTemporaryDir &data);
    QString qmlRoot() const;
    void createSystemQmlRoot(const QString &root) const;
};

void KnsCompatUnitTest::init()
{
    qunsetenv("LATTE_DISABLE_KNS_COMPAT");
    qunsetenv("LATTE_KNS_COMPAT_SYSTEM_QML_ROOTS");
    qunsetenv("LATTE_KNS_COMPAT_USER_QML_ROOT");
}

void KnsCompatUnitTest::cleanup()
{
    qunsetenv("LATTE_DISABLE_KNS_COMPAT");
    qunsetenv("LATTE_KNS_COMPAT_SYSTEM_QML_ROOTS");
    qunsetenv("LATTE_KNS_COMPAT_USER_QML_ROOT");
}

void KnsCompatUnitTest::isolateHomeAndData(QTemporaryDir &home, QTemporaryDir &data)
{
    QVERIFY(home.isValid());
    QVERIFY(data.isValid());

    qputenv("HOME", QFile::encodeName(home.path()));
    qputenv("XDG_DATA_HOME", QFile::encodeName(data.path()));
    QStandardPaths::setTestModeEnabled(false);
}

QString KnsCompatUnitTest::qmlRoot() const
{
    return QDir::homePath() + QStringLiteral("/.local/lib64/qt6/qml");
}

void KnsCompatUnitTest::createSystemQmlRoot(const QString &root) const
{
    const QString templates = root + QStringLiteral("/org/kde/kirigami/templates");
    const QString newstuff = root + QStringLiteral("/org/kde/newstuff");
    const QString controls = root + QStringLiteral("/org/kde/kirigami/controls");

    QVERIFY(QDir().mkpath(templates + QStringLiteral("/private")));
    QVERIFY(QDir().mkpath(newstuff + QStringLiteral("/private/actions")));
    QVERIFY(QDir().mkpath(controls + QStringLiteral("/private/globaltoolbar")));

    QFile templatesQml(templates + QStringLiteral("/Heading.qml"));
    QVERIFY(templatesQml.open(QFile::WriteOnly | QFile::Truncate));
    templatesQml.write("import QtQuick\nItem {}\n");
    templatesQml.close();

    QFile templatesPrivate(templates + QStringLiteral("/private/OverlayDrawerHandle.qml"));
    QVERIFY(templatesPrivate.open(QFile::WriteOnly | QFile::Truncate));
    templatesPrivate.write("import QtQuick\nItem {}\n");
    templatesPrivate.close();

    QFile newstuffQml(newstuff + QStringLiteral("/Button.qml"));
    QVERIFY(newstuffQml.open(QFile::WriteOnly | QFile::Truncate));
    newstuffQml.write("import QtQuick\nItem {}\n");
    newstuffQml.close();

    QFile newstuffPlugin(newstuff + QStringLiteral("/libnewstuffqmlplugin.so"));
    QVERIFY(newstuffPlugin.open(QFile::WriteOnly | QFile::Truncate));
    newstuffPlugin.close();

    QFile newstuffVersion(newstuff + QStringLiteral("/kde-qmlmodule.version"));
    QVERIFY(newstuffVersion.open(QFile::WriteOnly | QFile::Truncate));
    newstuffVersion.close();

    QFile newstuffPrivate(newstuff + QStringLiteral("/private/actions/Action.qml"));
    QVERIFY(newstuffPrivate.open(QFile::WriteOnly | QFile::Truncate));
    newstuffPrivate.write("import QtQuick\nItem {}\n");
    newstuffPrivate.close();

    QFile controlsQmldir(controls + QStringLiteral("/qmldir"));
    QVERIFY(controlsQmldir.open(QFile::WriteOnly | QFile::Truncate));
    controlsQmldir.write("module org.kde.kirigami.controls\nprefer :/qt/qml/org/kde/kirigami/controls\n");
    controlsQmldir.close();

    QFile controlsQml(controls + QStringLiteral("/ActionToolBar.qml"));
    QVERIFY(controlsQml.open(QFile::WriteOnly | QFile::Truncate));
    controlsQml.write("import QtQuick\nItem {}\n");
    controlsQml.close();

    QFile controlsPlugin(controls + QStringLiteral("/libKirigamiControlsplugin.so"));
    QVERIFY(controlsPlugin.open(QFile::WriteOnly | QFile::Truncate));
    controlsPlugin.close();

    QFile controlsVersion(controls + QStringLiteral("/kde-qmlmodule.version"));
    QVERIFY(controlsVersion.open(QFile::WriteOnly | QFile::Truncate));
    controlsVersion.close();

    QFile controlsPrivate(controls + QStringLiteral("/private/globaltoolbar/OtherButton.qml"));
    QVERIFY(controlsPrivate.open(QFile::WriteOnly | QFile::Truncate));
    controlsPrivate.write("import QtQuick\nItem {}\n");
    controlsPrivate.close();
}

void KnsCompatUnitTest::latestStampStillRepairsMissingControlsOverride()
{
    QTemporaryDir home;
    QTemporaryDir data;
    isolateHomeAndData(home, data);

    const QString stampDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
                             + QStringLiteral("/latte-dock-ng");
    QVERIFY(QDir().mkpath(stampDir));
    QFile stamp(stampDir + QStringLiteral("/kns-compat.stamp"));
    QVERIFY(stamp.open(QFile::WriteOnly | QFile::Truncate));
    QCOMPARE(stamp.write("7"), qint64(1));
    stamp.close();

    const QString root = qmlRoot();
    QVERIFY(QDir().mkpath(root + QStringLiteral("/org/kde/kirigami/templates")));
    QVERIFY(QDir().mkpath(root + QStringLiteral("/org/kde/newstuff")));

    QFile templatesQmldir(root + QStringLiteral("/org/kde/kirigami/templates/qmldir"));
    QVERIFY(templatesQmldir.open(QFile::WriteOnly | QFile::Truncate));
    templatesQmldir.close();

    QFile newstuffQmldir(root + QStringLiteral("/org/kde/newstuff/qmldir"));
    QVERIFY(newstuffQmldir.open(QFile::WriteOnly | QFile::Truncate));
    newstuffQmldir.close();

    ensureKnsCompat();

    QVERIFY(QFile::exists(root + QStringLiteral("/org/kde/kirigami/controls/qmldir")));
    QVERIFY(QFile::exists(root + QStringLiteral("/org/kde/kirigami/controls/private/globaltoolbar/HandleButton.qml")));
}

void KnsCompatUnitTest::firstRunWritesPatchedOverridesAndStamp()
{
    QTemporaryDir home;
    QTemporaryDir data;
    QTemporaryDir systemQml;
    isolateHomeAndData(home, data);
    QVERIFY(systemQml.isValid());
    createSystemQmlRoot(systemQml.path());
    qputenv("LATTE_KNS_COMPAT_SYSTEM_QML_ROOTS", QFile::encodeName(systemQml.path()));

    ensureKnsCompat();

    const QString root = qmlRoot();
    QFile drawer(root + QStringLiteral("/org/kde/kirigami/templates/private/DrawerHandle.qml"));
    QVERIFY(drawer.open(QFile::ReadOnly));
    const QByteArray drawerText = drawer.readAll();
    QVERIFY(drawerText.contains("onActiveTranslationChanged"));
    QVERIFY(!QFileInfo(drawer).isSymLink());

    QFile handle(root + QStringLiteral("/org/kde/kirigami/controls/private/globaltoolbar/HandleButton.qml"));
    QVERIFY(handle.open(QFile::ReadOnly));
    const QByteArray handleText = handle.readAll();
    QVERIFY(handleText.contains("onActiveTranslationChanged"));
    QVERIFY(!QFileInfo(handle).isSymLink());

    QFile controlsQmldir(root + QStringLiteral("/org/kde/kirigami/controls/qmldir"));
    QVERIFY(controlsQmldir.open(QFile::ReadOnly));
    QVERIFY(!controlsQmldir.readAll().contains("prefer "));

    QFile stamp(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
                + QStringLiteral("/latte-dock-ng/kns-compat.stamp"));
    QVERIFY(stamp.open(QFile::ReadOnly));
    QCOMPARE(stamp.readAll().trimmed(), QByteArray("7"));
}

void KnsCompatUnitTest::optOutDoesNotWriteOverrides()
{
    QTemporaryDir home;
    QTemporaryDir data;
    QTemporaryDir systemQml;
    isolateHomeAndData(home, data);
    QVERIFY(systemQml.isValid());
    createSystemQmlRoot(systemQml.path());
    qputenv("LATTE_KNS_COMPAT_SYSTEM_QML_ROOTS", QFile::encodeName(systemQml.path()));
    qputenv("LATTE_DISABLE_KNS_COMPAT", "1");

    ensureKnsCompat();

    QVERIFY(!QFile::exists(qmlRoot() + QStringLiteral("/org/kde/kirigami/templates/qmldir")));
    QVERIFY(!QFile::exists(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
                           + QStringLiteral("/latte-dock-ng/kns-compat.stamp")));
}

void KnsCompatUnitTest::usesConfiguredSystemQmlRoot()
{
    QTemporaryDir home;
    QTemporaryDir data;
    QTemporaryDir missingSystemQml;
    QTemporaryDir systemQml;
    isolateHomeAndData(home, data);
    QVERIFY(missingSystemQml.isValid());
    QVERIFY(systemQml.isValid());
    createSystemQmlRoot(systemQml.path());
    qputenv("LATTE_KNS_COMPAT_SYSTEM_QML_ROOTS",
            QFile::encodeName(missingSystemQml.path() + QLatin1Char(':') + systemQml.path()));

    ensureKnsCompat();

    QFile linkedNewstuff(qmlRoot() + QStringLiteral("/org/kde/newstuff/Button.qml"));
    QVERIFY(QFileInfo(linkedNewstuff).isSymLink());
    QCOMPARE(QFileInfo(linkedNewstuff).symLinkTarget(),
             systemQml.path() + QStringLiteral("/org/kde/newstuff/Button.qml"));
}

void KnsCompatUnitTest::usesConfiguredUserQmlRoot()
{
    QTemporaryDir home;
    QTemporaryDir data;
    QTemporaryDir systemQml;
    QTemporaryDir userQml;
    isolateHomeAndData(home, data);
    QVERIFY(systemQml.isValid());
    QVERIFY(userQml.isValid());
    createSystemQmlRoot(systemQml.path());
    qputenv("LATTE_KNS_COMPAT_SYSTEM_QML_ROOTS", QFile::encodeName(systemQml.path()));
    qputenv("LATTE_KNS_COMPAT_USER_QML_ROOT", QFile::encodeName(userQml.path()));

    ensureKnsCompat();

    QVERIFY(QFile::exists(userQml.path() + QStringLiteral("/org/kde/kirigami/templates/qmldir")));
    QVERIFY(QFile::exists(userQml.path() + QStringLiteral("/org/kde/newstuff/qmldir")));
    QVERIFY(QFile::exists(userQml.path() + QStringLiteral("/org/kde/kirigami/controls/qmldir")));
    QVERIFY(!QFile::exists(qmlRoot() + QStringLiteral("/org/kde/kirigami/templates/qmldir")));
}

void KnsCompatUnitTest::missingSystemQmlRootDoesNotCreatePartialOverrides()
{
    QTemporaryDir home;
    QTemporaryDir data;
    QTemporaryDir systemQml;
    isolateHomeAndData(home, data);
    QVERIFY(systemQml.isValid());
    qputenv("LATTE_KNS_COMPAT_SYSTEM_QML_ROOTS", QFile::encodeName(systemQml.path()));

    ensureKnsCompat();

    QVERIFY(!QFile::exists(qmlRoot() + QStringLiteral("/org/kde/kirigami/templates/qmldir")));
    QVERIFY(!QFile::exists(qmlRoot() + QStringLiteral("/org/kde/newstuff/qmldir")));
    QVERIFY(!QFile::exists(qmlRoot() + QStringLiteral("/org/kde/kirigami/controls/qmldir")));
}

QTEST_GUILESS_MAIN(KnsCompatUnitTest)

#include "knscompatunittest.moc"
