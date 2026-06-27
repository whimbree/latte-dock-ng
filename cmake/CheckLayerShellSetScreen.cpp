/*
    SPDX-FileCopyrightText: 2026 Ruizhi Zhong <ruizhi.zhong@outlook.com>
    SPDX-License-Identifier: GPL-2.0-or-later

    CMake try_compile probe — detects whether LayerShellQt::Window exposes
    setScreen(QScreen*), which was added in LayerShellQt 6.6 and later
    removed upstream.

    If this file compiles, CMake defines LATTE_LAYERSHELL_HAS_SET_SCREEN
    so that waylandinterface.cpp can conditionally call the method.
*/

#include <LayerShellQt/window.h>

int main()
{
    LayerShellQt::Window *w = nullptr;
    w->setScreen(nullptr);
    return 0;
}
