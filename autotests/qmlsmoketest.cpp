/*
    SPDX-FileCopyrightText: 2026 Ruizhi Zhong <ruizhi.zhong88@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>

#include <QDir>
#include <QFile>
#include <QMetaObject>
#include <QTemporaryDir>
#include <QTest>

#include <memory>

class QmlSmokeTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void latteCoreQmlPluginLoadsFromBuildTree();
    void restoreAnimationLoadsFromSource();
    void showWindowAnimationFrozenZoomDecisionLoadsFromSource();
};

class ParabolicTargetStub : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)

public:
    qreal zoom() const
    {
        return m_zoom;
    }

    void setZoom(qreal zoom)
    {
        if (qFuzzyCompare(m_zoom, zoom)) {
            return;
        }

        m_zoom = zoom;
        Q_EMIT zoomChanged();
    }

Q_SIGNALS:
    void zoomChanged();

private:
    qreal m_zoom{1.5};
};

class AbilityItemStub : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *parabolicItem READ parabolicItem CONSTANT)
    Q_PROPERTY(int animationTime READ animationTime CONSTANT)

public:
    explicit AbilityItemStub(QObject *parabolicItem, QObject *parent = nullptr)
        : QObject(parent)
        , m_parabolicItem(parabolicItem)
    {
    }

    QObject *parabolicItem() const
    {
        return m_parabolicItem;
    }

    int animationTime() const
    {
        return 10;
    }

private:
    QObject *m_parabolicItem{nullptr};
};

class TaskItemStub : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *parabolicItem READ parabolicItem CONSTANT)
    Q_PROPERTY(QVariantMap abilities READ abilities CONSTANT)
    Q_PROPERTY(bool parabolicAreaIsCurrent READ parabolicAreaIsCurrent WRITE setParabolicAreaIsCurrent NOTIFY parabolicAreaChanged)
    Q_PROPERTY(bool parabolicAreaContainsMouse READ parabolicAreaContainsMouse WRITE setParabolicAreaContainsMouse NOTIFY parabolicAreaChanged)
    Q_PROPERTY(bool isVertical READ isVertical CONSTANT)
    Q_PROPERTY(bool isWindow READ isWindow CONSTANT)
    Q_PROPERTY(bool isStartup READ isStartup CONSTANT)
    Q_PROPERTY(bool isLauncher READ isLauncher CONSTANT)
    Q_PROPERTY(bool isSeparator READ isSeparator CONSTANT)
    Q_PROPERTY(QString launcherUrl READ launcherUrl CONSTANT)
    Q_PROPERTY(QString launcherUrlWithIcon READ launcherUrlWithIcon CONSTANT)
    Q_PROPERTY(qreal iconAnimatedOffsetX MEMBER m_iconAnimatedOffsetX)
    Q_PROPERTY(qreal iconAnimatedOffsetY MEMBER m_iconAnimatedOffsetY)
    Q_PROPERTY(bool inAddRemoveAnimation MEMBER m_inAddRemoveAnimation)
    Q_PROPERTY(bool inAnimation MEMBER m_inAnimation)

public:
    explicit TaskItemStub(QObject *parabolicItem, QObject *parent = nullptr)
        : QObject(parent)
        , m_parabolicItem(parabolicItem)
    {
    }

    QObject *parabolicItem() const
    {
        return m_parabolicItem;
    }

    QVariantMap abilities() const
    {
        return QVariantMap{
            {QStringLiteral("animations"),
             QVariantMap{
                 {QStringLiteral("speedFactor"), QVariantMap{{QStringLiteral("normal"), 1}, {QStringLiteral("current"), 1}}},
                 {QStringLiteral("duration"), QVariantMap{{QStringLiteral("large"), 100}}},
                 {QStringLiteral("needLength"), QVariant::fromValue(static_cast<QObject *>(const_cast<TaskItemStub *>(this)))},
             }},
            {QStringLiteral("metrics"), QVariantMap{{QStringLiteral("iconSize"), 48}}},
            {QStringLiteral("launchers"), QVariant::fromValue(static_cast<QObject *>(const_cast<TaskItemStub *>(this)))},
        };
    }

    bool parabolicAreaIsCurrent() const
    {
        return m_parabolicAreaIsCurrent;
    }

    void setParabolicAreaIsCurrent(bool current)
    {
        m_parabolicAreaIsCurrent = current;
        Q_EMIT parabolicAreaChanged();
    }

    bool parabolicAreaContainsMouse() const
    {
        return m_parabolicAreaContainsMouse;
    }

    void setParabolicAreaContainsMouse(bool containsMouse)
    {
        m_parabolicAreaContainsMouse = containsMouse;
        Q_EMIT parabolicAreaChanged();
    }

    bool isVertical() const
    {
        return false;
    }

    bool isWindow() const
    {
        return true;
    }

    bool isStartup() const
    {
        return false;
    }

    bool isLauncher() const
    {
        return false;
    }

    bool isSeparator() const
    {
        return false;
    }

    QString launcherUrl() const
    {
        return QStringLiteral("applications:test.desktop");
    }

    QString launcherUrlWithIcon() const
    {
        return launcherUrl();
    }

    Q_INVOKABLE void addEvent(const QString &)
    {
    }

    Q_INVOKABLE void removeEvent(const QString &)
    {
    }

    Q_INVOKABLE bool inCurrentActivity(const QString &) const
    {
        return true;
    }

Q_SIGNALS:
    void parabolicAreaChanged();

private:
    QObject *m_parabolicItem{nullptr};
    bool m_parabolicAreaIsCurrent{false};
    bool m_parabolicAreaContainsMouse{false};
    qreal m_iconAnimatedOffsetX{0};
    qreal m_iconAnimatedOffsetY{0};
    bool m_inAddRemoveAnimation{false};
    bool m_inAnimation{false};
};

class RootStub : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool newWindowSlidingEnabled READ newWindowSlidingEnabled CONSTANT)
    Q_PROPERTY(bool vertical READ vertical CONSTANT)
    Q_PROPERTY(bool inActivityChange READ inActivityChange CONSTANT)
    Q_PROPERTY(bool inDraggingPhase READ inDraggingPhase CONSTANT)
    Q_PROPERTY(bool showWindowsOnlyFromLaunchers READ showWindowsOnlyFromLaunchers CONSTANT)
    Q_PROPERTY(bool disableAllWindowsFunctionality READ disableAllWindowsFunctionality CONSTANT)

public:
    bool newWindowSlidingEnabled() const { return true; }
    bool vertical() const { return false; }
    bool inActivityChange() const { return false; }
    bool inDraggingPhase() const { return false; }
    bool showWindowsOnlyFromLaunchers() const { return false; }
    bool disableAllWindowsFunctionality() const { return false; }
};

class TasksExtendedManagerStub : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE bool toBeAddedLauncherExists(const QString &) const { return false; }
    Q_INVOKABLE void removeToBeAddedLauncher(const QString &) {}
    Q_INVOKABLE bool immediateLauncherExists(const QString &) const { return false; }
    Q_INVOKABLE void removeImmediateLauncher(const QString &) {}
    Q_INVOKABLE QVariantMap getFrozenTask(const QString &) const { return QVariantMap{{QStringLiteral("zoom"), 1.5}}; }
    Q_INVOKABLE void removeFrozenTask(const QString &) {}
};

class TasksModelStub : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE int launcherPosition(const QString &) const { return -1; }
    Q_INVOKABLE QStringList launcherActivities(const QString &) const { return {}; }
};

static void addLatteCoreImport(QQmlEngine &engine, QTemporaryDir &importRoot)
{
    QVERIFY(importRoot.isValid());

    const QString modulePath = importRoot.path() + QStringLiteral("/org/kde/latte/core");
    QVERIFY(QDir().mkpath(modulePath));
    QVERIFY(QFile::copy(QStringLiteral(LATTE_CORE_QMLDIR), modulePath + QStringLiteral("/qmldir")));
    QVERIFY(QFile::copy(QStringLiteral(LATTE_CORE_PLUGIN), modulePath + QStringLiteral("/liblattecoreplugin.so")));

    engine.addImportPath(importRoot.path());
}

void QmlSmokeTest::latteCoreQmlPluginLoadsFromBuildTree()
{
    QTemporaryDir importRoot;
    QQmlEngine engine;
    addLatteCoreImport(engine, importRoot);

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

void QmlSmokeTest::restoreAnimationLoadsFromSource()
{
    QQmlEngine engine;
    QQmlContext context(engine.rootContext());
    ParabolicTargetStub parabolicTarget;
    AbilityItemStub abilityItem(&parabolicTarget);
    context.setContextProperty(QStringLiteral("abilityItem"), &abilityItem);

    QQmlComponent component(&engine, QUrl::fromLocalFile(QStringLiteral(LATTE_RESTORE_ANIMATION_QML)));
    std::unique_ptr<QObject> object(component.create(&context));
    if (!object) {
        qWarning() << component.errors();
    }

    QVERIFY(object);
    const auto animations = object->findChildren<QObject *>();
    QObject *zoomAnimation = nullptr;
    for (QObject *animation : animations) {
        if (animation->property("property").toString() == QStringLiteral("zoom")) {
            zoomAnimation = animation;
            break;
        }
    }

    QVERIFY(zoomAnimation);
    QCOMPARE(zoomAnimation->property("target").value<QObject *>(), &parabolicTarget);
    QCOMPARE(zoomAnimation->property("to").toReal(), 1.0);
}

void QmlSmokeTest::showWindowAnimationFrozenZoomDecisionLoadsFromSource()
{
    QTemporaryDir importRoot;
    QQmlEngine engine;
    addLatteCoreImport(engine, importRoot);

    QQmlContext context(engine.rootContext());
    ParabolicTargetStub parabolicTarget;
    TaskItemStub taskItem(&parabolicTarget);
    RootStub root;
    TasksExtendedManagerStub tasksExtendedManager;
    TasksModelStub tasksModel;
    QObject publishGeometryTimer;
    QVariantMap activityInfo{
        {QStringLiteral("currentActivity"), QStringLiteral("current")},
        {QStringLiteral("previousActivity"), QStringLiteral("previous")},
    };

    context.setContextProperty(QStringLiteral("taskItem"), &taskItem);
    context.setContextProperty(QStringLiteral("root"), &root);
    context.setContextProperty(QStringLiteral("tasksExtendedManager"), &tasksExtendedManager);
    context.setContextProperty(QStringLiteral("tasksModel"), &tasksModel);
    context.setContextProperty(QStringLiteral("activityInfo"), activityInfo);
    context.setContextProperty(QStringLiteral("publishGeometryTimer"), &publishGeometryTimer);
    context.setContextProperty(QStringLiteral("isWindow"), true);
    context.setContextProperty(QStringLiteral("isLauncher"), false);
    context.setContextProperty(QStringLiteral("isForcedHidden"), false);
    context.setContextProperty(QStringLiteral("index"), QVariant::fromValue(0));
    context.setContextProperty(QStringLiteral("icList"), QVariantMap{});

    QQmlComponent component(&engine, QUrl::fromLocalFile(QStringLiteral(LATTE_SHOW_WINDOW_ANIMATION_QML)));
    std::unique_ptr<QObject> object(component.create(&context));
    if (!object) {
        qWarning() << component.errors();
    }

    QVERIFY(object);
    QVariant keepFrozenZoom;
    QVERIFY(QMetaObject::invokeMethod(object.get(), "keepFrozenZoomForCurrentTask", Q_RETURN_ARG(QVariant, keepFrozenZoom)));
    QCOMPARE(keepFrozenZoom.toBool(), false);

    taskItem.setParabolicAreaContainsMouse(true);
    QVERIFY(QMetaObject::invokeMethod(object.get(), "keepFrozenZoomForCurrentTask", Q_RETURN_ARG(QVariant, keepFrozenZoom)));
    QCOMPARE(keepFrozenZoom.toBool(), true);

    taskItem.setParabolicAreaContainsMouse(false);
    taskItem.setParabolicAreaIsCurrent(true);
    QVERIFY(QMetaObject::invokeMethod(object.get(), "keepFrozenZoomForCurrentTask", Q_RETURN_ARG(QVariant, keepFrozenZoom)));
    QCOMPARE(keepFrozenZoom.toBool(), true);
}

QTEST_MAIN(QmlSmokeTest)

#include "qmlsmoketest.moc"
