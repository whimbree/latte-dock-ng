/*
    SPDX-FileCopyrightText: 2026 Ruizhi Zhong <ruizhi.zhong88@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "schemecolors.h"
#include "importer.h"

#include <KConfigGroup>
#include <KSharedConfig>

#include <QDir>
#include <QFile>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

namespace {
QString s_configPath;
QString s_dataRoot;

QString writeScheme(const QString &path, const QString &name)
{
    QDir().mkpath(QFileInfo(path).absolutePath());

    KSharedConfigPtr config = KSharedConfig::openConfig(path);
    KConfigGroup general(config, "General");
    general.writeEntry("Name", name);

    KConfigGroup wm(config, "WM");
    wm.writeEntry("activeBackground", QColor(1, 2, 3));
    wm.writeEntry("activeForeground", QColor(4, 5, 6));
    wm.writeEntry("inactiveBackground", QColor(7, 8, 9));
    wm.writeEntry("inactiveForeground", QColor(10, 11, 12));

    KConfigGroup selection(config, "Colors:Selection");
    selection.writeEntry("BackgroundNormal", QColor(13, 14, 15));
    selection.writeEntry("ForegroundNormal", QColor(16, 17, 18));

    KConfigGroup window(config, "Colors:Window");
    window.writeEntry("BackgroundNormal", QColor(19, 20, 21));
    window.writeEntry("ForegroundNormal", QColor(22, 23, 24));
    window.writeEntry("BackgroundAlternate", QColor(25, 26, 27));
    window.writeEntry("ForegroundInactive", QColor(28, 29, 30));
    window.writeEntry("ForegroundPositive", QColor(31, 32, 33));
    window.writeEntry("ForegroundNeutral", QColor(34, 35, 36));
    window.writeEntry("ForegroundNegative", QColor(37, 38, 39));

    KConfigGroup button(config, "Colors:Button");
    button.writeEntry("ForegroundNormal", QColor(40, 41, 42));
    button.writeEntry("BackgroundNormal", QColor(43, 44, 45));
    button.writeEntry("DecorationHover", QColor(46, 47, 48));
    button.writeEntry("DecorationFocus", QColor(49, 50, 51));

    config->sync();
    return path;
}
}

namespace Latte {
QString configPath()
{
    return s_configPath;
}

namespace Layouts {
QString Importer::standardPath(QString subPath, bool localfirst)
{
    Q_UNUSED(localfirst)

    const QString path = s_dataRoot + QStringLiteral("/") + subPath;
    return QFileInfo::exists(path) ? path : QString();
}
}
}

class SchemeColorsUnitTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void schemeNameReadsConfigNameOrFallsBackToFileName();
    void possibleSchemeFileFindsExactAndSimplifiedNames();
    void possibleSchemeFileResolvesAutoAccentKdeglobals();
    void schemeColorsLoadsWindowManagerColors();
    void schemeColorsCanLoadPlasmaThemeColors();
};

void SchemeColorsUnitTest::schemeNameReadsConfigNameOrFallsBackToFileName()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString named = writeScheme(dir.path() + QStringLiteral("/Named.colors"), QStringLiteral("Readable Name"));
    QCOMPARE(Latte::WindowSystem::SchemeColors::schemeName(named), QStringLiteral("Readable Name"));

    const QString fallback = dir.path() + QStringLiteral("/Fallback.colors");
    QDir().mkpath(QFileInfo(fallback).absolutePath());
    QFile fallbackFile(fallback);
    QVERIFY(fallbackFile.open(QIODevice::WriteOnly | QIODevice::Truncate));
    fallbackFile.close();
    QCOMPARE(Latte::WindowSystem::SchemeColors::schemeName(fallback), QStringLiteral("Fallback"));

    QCOMPARE(Latte::WindowSystem::SchemeColors::schemeName(dir.path() + QStringLiteral("/NotColors.txt")), QString());
}

void SchemeColorsUnitTest::possibleSchemeFileFindsExactAndSimplifiedNames()
{
    QTemporaryDir dataRoot;
    QVERIFY(dataRoot.isValid());
    s_dataRoot = dataRoot.path();

    const QString exact = writeScheme(dataRoot.path() + QStringLiteral("/color-schemes/Exact.colors"), QStringLiteral("Exact"));
    QCOMPARE(Latte::WindowSystem::SchemeColors::possibleSchemeFile(exact), exact);
    QCOMPARE(Latte::WindowSystem::SchemeColors::possibleSchemeFile(QStringLiteral("Exact")), exact);

    const QString simplified = writeScheme(dataRoot.path() + QStringLiteral("/color-schemes/MyScheme.colors"), QStringLiteral("My Scheme"));
    QCOMPARE(Latte::WindowSystem::SchemeColors::possibleSchemeFile(QStringLiteral("My Scheme")), simplified);
    QCOMPARE(Latte::WindowSystem::SchemeColors::possibleSchemeFile(QStringLiteral("My-Scheme")), simplified);
}

void SchemeColorsUnitTest::possibleSchemeFileResolvesAutoAccentKdeglobals()
{
    QTemporaryDir configRoot;
    QVERIFY(configRoot.isValid());
    s_configPath = configRoot.path();

    QFile kdeglobals(configRoot.path() + QStringLiteral("/kdeglobals"));
    QVERIFY(kdeglobals.open(QIODevice::WriteOnly | QIODevice::Truncate));
    kdeglobals.write("[WM]\nactiveBackground=1,2,3\n");
    kdeglobals.close();

    QCOMPARE(Latte::WindowSystem::SchemeColors::possibleSchemeFile(QStringLiteral("kdeglobals")),
             configRoot.path() + QStringLiteral("/kdeglobals"));
}

void SchemeColorsUnitTest::schemeColorsLoadsWindowManagerColors()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString scheme = writeScheme(dir.path() + QStringLiteral("/Colors.colors"), QStringLiteral("Colors"));
    Latte::WindowSystem::SchemeColors colors(nullptr, scheme);
    QSignalSpy schemeFileSpy(&colors, &Latte::WindowSystem::SchemeColors::schemeFileChanged);
    colors.setSchemeFile(scheme);

    QCOMPARE(colors.schemeFile(), scheme);
    QCOMPARE(colors.schemeName(), QStringLiteral("Colors"));
    QCOMPARE(colors.backgroundColor(), QColor(1, 2, 3));
    QCOMPARE(colors.textColor(), QColor(4, 5, 6));
    QCOMPARE(colors.inactiveBackgroundColor(), QColor(7, 8, 9));
    QCOMPARE(colors.inactiveTextColor(), QColor(10, 11, 12));
    QCOMPARE(colors.highlightColor(), QColor(13, 14, 15));
    QCOMPARE(colors.highlightedTextColor(), QColor(16, 17, 18));
    QCOMPARE(colors.positiveTextColor(), QColor(31, 32, 33));
    QCOMPARE(colors.buttonFocusColor(), QColor(49, 50, 51));
    QCOMPARE(schemeFileSpy.count(), 0);
}

void SchemeColorsUnitTest::schemeColorsCanLoadPlasmaThemeColors()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString scheme = writeScheme(dir.path() + QStringLiteral("/Plasma.colors"), QStringLiteral("Plasma"));
    Latte::WindowSystem::SchemeColors colors(nullptr, scheme, true);

    QCOMPARE(colors.backgroundColor(), QColor(19, 20, 21));
    QCOMPARE(colors.textColor(), QColor(22, 23, 24));
    QCOMPARE(colors.inactiveBackgroundColor(), QColor(25, 26, 27));
    QCOMPARE(colors.inactiveTextColor(), QColor(28, 29, 30));
}

QTEST_GUILESS_MAIN(SchemeColorsUnitTest)

#include "schemecolorsunittest.moc"
