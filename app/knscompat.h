/*
    SPDX-FileCopyrightText: 2026 Ruizhi Zhong <ruizhi.zhong88@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KNSCOMPAT_H
#define KNSCOMPAT_H

#include <QString>
#include <QStringList>

class QQmlEngine;

//! Set up user-local QML module overrides to fix the KNS download dialog.
//! Qt 6.10.3 removed DragHandler.xAxis.onActiveValueChanged, which breaks
//! Kirigami's DrawerHandle.qml.  The system qmldir files use `prefer` to
//! load incompatible AOT-compiled versions.  We create user-local module
//! overrides without `prefer` and a patched DrawerHandle.qml so that the
//! KNSWidgets::Dialog (opened by "Download New Plasma Widgets") renders
//! correctly, matching the official Plasma panel behavior.
void ensureKnsCompat();

//! Returns the user-local QML import root used by ensureKnsCompat(), or an
//! empty string when no complete system QML root can be found.
QString knsCompatUserQmlRoot();

//! Returns user-local QML import paths collected at startup (for user-mode
//! installs only).  These paths contain latte-dock's own QML modules and are
//! meant to be added per-engine via addImportPath() rather than exported
//! globally through environment variables.
const QStringList &userLocalQmlImportPaths();

//! Add all latte-dock QML import paths (user-local modules + KNS compat)
//! to the given QQmlEngine.  This is the engine-scoped replacement for the
//! old approach of setting QML_IMPORT_PATH / QT_QML_IMPORT_PATH env vars
//! that leaked into child processes and could crash other Qt applications.
void addLatteQmlImportPaths(QQmlEngine *engine);

#endif // KNSCOMPAT_H
