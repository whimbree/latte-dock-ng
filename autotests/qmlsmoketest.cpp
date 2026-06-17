/*
    SPDX-FileCopyrightText: 2026 Ruizhi Zhong <ruizhi.zhong88@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QQmlComponent>
#include <QQmlEngine>

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QTest>

class QmlSmokeTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void latteCoreQmlPluginLoadsFromBuildTree();
    void pulseAudioBootstrapIsBounded();
    void compactAppletUsesLargerDefaultForVolumePopup();
    void compactAppletKeepsApplicationMenuPopupResizable();
    void applicationLauncherUsesFixedExternalSlot();
    void latteTasksExposesPlasmaLauncherApi();
    void latteDockDbusExportsLauncherApi();
};

void QmlSmokeTest::latteCoreQmlPluginLoadsFromBuildTree()
{
    QTemporaryDir importRoot;
    QVERIFY(importRoot.isValid());

    const QString modulePath = importRoot.path() + QStringLiteral("/org/kde/latte/core");
    QVERIFY(QDir().mkpath(modulePath));
    QVERIFY(QFile::copy(QStringLiteral(LATTE_CORE_QMLDIR), modulePath + QStringLiteral("/qmldir")));
    QVERIFY(QFile::copy(QStringLiteral(LATTE_CORE_PLUGIN), modulePath + QStringLiteral("/liblattecoreplugin.so")));

    QQmlEngine engine;
    engine.addImportPath(importRoot.path());

    QQmlComponent component(&engine);
    component.setData(R"(
import QtQml 2.15
import org.kde.latte.core 0.2

QtObject {
    property int separator: Environment.separatorLength
    property int version: Environment.makeVersion(1, 2, 3)
    property real brightness: Tools.colorBrightness("#ffffff")
    property real lumina: Tools.colorLumina("#000000")
    property bool hasCompositingProperty: WindowSystem.compositingActive === true || WindowSystem.compositingActive === false
}
)",
                      QUrl(QStringLiteral("qrc:/lattecore-smoke.qml")));

    std::unique_ptr<QObject> object(component.create());
    if (!object) {
        qWarning() << component.errors();
    }

    QVERIFY(object);
    QCOMPARE(object->property("separator").toInt(), 5);
    QCOMPARE(object->property("version").toInt(), 0x010203);
    QVERIFY(object->property("brightness").toReal() > 0.99);
    QCOMPARE(object->property("lumina").toReal(), 0.0);
    QCOMPARE(object->property("hasCompositingProperty").toBool(), true);
}

void QmlSmokeTest::pulseAudioBootstrapIsBounded()
{
    QFile pulseAudio(QStringLiteral(LATTE_SOURCE_DIR "/plasmoid/package/contents/ui/PulseAudio.qml"));
    QVERIFY(pulseAudio.open(QFile::ReadOnly));

    // Regression lock: the PulseAudio refresh must be a bounded startup
    // bootstrap, not a long-running periodic fix timer.
    const QString source = QString::fromUtf8(pulseAudio.readAll());
    QVERIFY(source.contains(QStringLiteral("bootstrapAttempts")));
    QVERIFY(source.contains(QStringLiteral("bootstrapMaxAttempts")));
    QVERIFY(source.contains(QStringLiteral("paFixTimer.stop()")));
    QVERIFY(!source.contains(QStringLiteral("interval = 30000")));
}

void QmlSmokeTest::compactAppletUsesLargerDefaultForVolumePopup()
{
    QFile compactApplet(QStringLiteral(LATTE_SOURCE_DIR "/shell/package/contents/applet/CompactApplet.qml"));
    QVERIFY(compactApplet.open(QFile::ReadOnly));

    // Regression lock: the Plasma volume applet needs Latte's wrapper to keep
    // a Plasma-panel-like initial popup size instead of reusing stale tiny
    // persisted popup dimensions.
    const QString source = QString::fromUtf8(compactApplet.readAll());
    QVERIFY(source.contains(QStringLiteral("function popupPreferredWidth")));
    QVERIFY(source.contains(QStringLiteral("function popupPreferredHeight")));
    QVERIFY(source.contains(QStringLiteral("org.kde.plasma.volume")));
    QVERIFY(source.contains(QStringLiteral("Kirigami.Units.gridUnit * 27")));
}

void QmlSmokeTest::compactAppletKeepsApplicationMenuPopupResizable()
{
    QFile compactApplet(QStringLiteral(LATTE_SOURCE_DIR "/shell/package/contents/applet/CompactApplet.qml"));
    QVERIFY(compactApplet.open(QFile::ReadOnly));

    // Regression lock: application menu popups should keep their preferred
    // initial size while advertising a smaller minimum and unbounded maximum
    // so the user can resize both width and height.
    const QString source = QString::fromUtf8(compactApplet.readAll());
    QVERIFY(source.contains(QStringLiteral("function popupMenuMinimumWidth")));
    QVERIFY(source.contains(QStringLiteral("function popupMenuMinimumHeight")));
    QVERIFY(source.contains(QStringLiteral("function popupMaximumWidth")));
    QVERIFY(source.contains(QStringLiteral("function popupMaximumHeight")));
    QVERIFY(source.contains(QStringLiteral("org.kde.plasma.kicker")));
    QVERIFY(source.contains(QStringLiteral("isApplicationMenuApplet()")));
    QVERIFY(source.contains(QStringLiteral("Kirigami.Units.gridUnit * 18")));
    QVERIFY(source.contains(QStringLiteral("return Infinity;")));
}

void QmlSmokeTest::applicationLauncherUsesFixedExternalSlot()
{
    QFile appletItem(QStringLiteral(LATTE_SOURCE_DIR "/containment/package/contents/ui/applet/AppletItem.qml"));
    QVERIFY(appletItem.open(QFile::ReadOnly));

    const QString source = QString::fromUtf8(appletItem.readAll());
    QVERIFY(source.contains(QStringLiteral("isApplicationLauncherApplet")));
    QVERIFY(source.contains(QStringLiteral("org.kde.plasma.kickoff")));
    QVERIFY(source.contains(QStringLiteral("|| (!communicator.appletMainIconIsFound")));
}

void QmlSmokeTest::latteTasksExposesPlasmaLauncherApi()
{
    QFile latteTasks(QStringLiteral(LATTE_SOURCE_DIR "/plasmoid/package/contents/ui/main.qml"));
    QVERIFY(latteTasks.open(QFile::ReadOnly));

    const QString source = QString::fromUtf8(latteTasks.readAll());
    QVERIFY(source.contains(QStringLiteral("readonly property bool supportsLaunchers: true")));
    QVERIFY(source.contains(QStringLiteral("function hasLauncher(url)")));
    QVERIFY(source.contains(QStringLiteral("appletAbilities.launchers.hasLauncher(url)")));
    QVERIFY(source.contains(QStringLiteral("function addLauncher(url)")));
    QVERIFY(source.contains(QStringLiteral("appletAbilities.launchers.addLauncher(url)")));
    QVERIFY(source.contains(QStringLiteral("function removeLauncher(url)")));
    QVERIFY(source.contains(QStringLiteral("appletAbilities.launchers.removeLauncher(url)")));
}

void QmlSmokeTest::latteDockDbusExportsLauncherApi()
{
    QFile dbusXml(QStringLiteral(LATTE_SOURCE_DIR "/app/dbus/org.kde.LatteDock.xml"));
    QVERIFY(dbusXml.open(QFile::ReadOnly));

    const QString xml = QString::fromUtf8(dbusXml.readAll());
    QVERIFY(xml.contains(QStringLiteral("<method name=\"hasLauncher\">")));
    QVERIFY(xml.contains(QStringLiteral("<method name=\"addLauncher\">")));
    QVERIFY(xml.contains(QStringLiteral("<method name=\"removeLauncher\">")));
    QVERIFY(xml.contains(QStringLiteral("<arg name=\"launcherUrl\" type=\"s\" direction=\"in\"/>")));
    QVERIFY(xml.contains(QStringLiteral("<arg name=\"screenName\" type=\"s\" direction=\"in\"/>")));
    QVERIFY(xml.contains(QStringLiteral("<arg name=\"success\" type=\"b\" direction=\"out\"/>")));

    QFile coronaHeader(QStringLiteral(LATTE_SOURCE_DIR "/app/lattecorona.h"));
    QVERIFY(coronaHeader.open(QFile::ReadOnly));

    const QString header = QString::fromUtf8(coronaHeader.readAll());
    QVERIFY(header.contains(QStringLiteral("bool hasLauncher(QString launcherUrl, QString screenName);")));
    QVERIFY(header.contains(QStringLiteral("bool addLauncher(QString launcherUrl, QString screenName);")));
    QVERIFY(header.contains(QStringLiteral("bool removeLauncher(QString launcherUrl, QString screenName);")));
}

QTEST_MAIN(QmlSmokeTest)

#include "qmlsmoketest.moc"
