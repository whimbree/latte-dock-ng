/*
    SPDX-FileCopyrightText: 2026 Ruizhi Zhong <ruizhi.zhong88@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QFile>
#include <QTest>

class PackagingContractTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void distroInstallPackagingContractsStayInSync();
};

void PackagingContractTest::distroInstallPackagingContractsStayInSync()
{
    QFile uninstallScript(QStringLiteral(LATTE_SOURCE_DIR "/uninstall.sh"));
    QVERIFY(uninstallScript.open(QFile::ReadOnly));
    const QString uninstallSource = QString::fromUtf8(uninstallScript.readAll());
    QVERIFY(uninstallSource.contains(QStringLiteral("org.kde.latte.contextmenu.so")));
    QVERIFY(uninstallSource.contains(QStringLiteral("plasma_containmentactions_lattecontextmenu.so")));
    QVERIFY(uninstallSource.contains(QStringLiteral("lib/x86_64-linux-gnu/qt6/qml")));

    QFile installScript(QStringLiteral(LATTE_SOURCE_DIR "/install.sh"));
    QVERIFY(installScript.open(QFile::ReadOnly));
    const QString installSource = QString::fromUtf8(installScript.readAll());
    QVERIFY(installSource.contains(QStringLiteral("--build-dir <path>")));
    QVERIFY(installSource.contains(QStringLiteral("LATTE_BUILD_DIR")));
    QVERIFY(installSource.contains(QStringLiteral("lib/x86_64-linux-gnu/qt6/qml")));

    QFile mainSourceFile(QStringLiteral(LATTE_SOURCE_DIR "/app/main.cpp"));
    QVERIFY(mainSourceFile.open(QFile::ReadOnly));
    const QString mainSource = QString::fromUtf8(mainSourceFile.readAll());
    QVERIFY(mainSource.contains(QStringLiteral("lib/x86_64-linux-gnu/qt6/qml")));

    QFile knsCompat(QStringLiteral(LATTE_SOURCE_DIR "/app/knscompat.cpp"));
    QVERIFY(knsCompat.open(QFile::ReadOnly));
    const QString knsCompatSource = QString::fromUtf8(knsCompat.readAll());
    QVERIFY(knsCompatSource.contains(QStringLiteral("userLocalQmlBase")));
    QVERIFY(!knsCompatSource.contains(QStringLiteral("QDir::homePath() + QStringLiteral(\"/.local/lib64/qt6/qml\")")));

    QFile dockerCompose(QStringLiteral(LATTE_SOURCE_DIR "/docker/docker-compose.yml"));
    QVERIFY(dockerCompose.open(QFile::ReadOnly));
    const QString dockerSource = QString::fromUtf8(dockerCompose.readAll());
    QVERIFY(dockerSource.contains(QStringLiteral("${LATTE_SRC:-..}:/src:ro")));
    QVERIFY(dockerSource.contains(QStringLiteral("bash /src/docker/verify-install.sh")));
    QVERIFY(!dockerSource.contains(QStringLiteral("/data/projects/latte-dock-ng:/src:ro")));

    QFile dockerVerify(QStringLiteral(LATTE_SOURCE_DIR "/docker/verify-install.sh"));
    QVERIFY(dockerVerify.open(QFile::ReadOnly));
    const QString dockerVerifySource = QString::fromUtf8(dockerVerify.readAll());
    QVERIFY(dockerVerifySource.contains(QStringLiteral("install.sh --system --build-dir")));
    QVERIFY(dockerVerifySource.contains(QStringLiteral("install.sh --user --build-dir")));
    QVERIFY(dockerVerifySource.contains(QStringLiteral("manifestless uninstall fallback")));

    QFile archPackage(QStringLiteral(LATTE_SOURCE_DIR "/docker/package-arch.sh"));
    QVERIFY(archPackage.open(QFile::ReadOnly));
    const QString archPackageSource = QString::fromUtf8(archPackage.readAll());
    QVERIFY(archPackageSource.contains(QStringLiteral("depend = kirigami")));
    QVERIFY(archPackageSource.contains(QStringLiteral("depend = kcmutils")));
    QVERIFY(!archPackageSource.contains(QStringLiteral("depend = kf6-kirigami")));

    QFile archDockerfile(QStringLiteral(LATTE_SOURCE_DIR "/docker/Dockerfile.arch"));
    QVERIFY(archDockerfile.open(QFile::ReadOnly));
    const QString archDockerfileSource = QString::fromUtf8(archDockerfile.readAll());
    QVERIFY(archDockerfileSource.contains(QStringLiteral("zstd")));

    QFile packagingCMake(QStringLiteral(LATTE_SOURCE_DIR "/cmake/LattePackaging.cmake"));
    QVERIFY(packagingCMake.open(QFile::ReadOnly));
    const QString packagingCMakeSource = QString::fromUtf8(packagingCMake.readAll());
    QVERIFY(packagingCMakeSource.contains(QStringLiteral("kf6-kcmutils")));
    QVERIFY(packagingCMakeSource.contains(QStringLiteral("qml6-module-org-kde-kcmutils")));
}

QTEST_MAIN(PackagingContractTest)

#include "packagingcontracttest.moc"
