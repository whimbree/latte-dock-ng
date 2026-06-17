/*
    SPDX-FileCopyrightText: 2026 Ruizhi Zhong <ruizhi.zhong88@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "layoutmenuitemwidget.h"

#include <QAction>
#include <QRadioButton>
#include <QTest>

class ContextMenuUnitTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void layoutMenuItemReflectsCheckableActionState();
    void layoutMenuItemHidesRadioForNonCheckableAction();
};

void ContextMenuUnitTest::layoutMenuItemReflectsCheckableActionState()
{
    QAction action(QStringLiteral("<b>Current Layout</b>"), this);
    action.setCheckable(true);
    action.setChecked(true);
    action.setVisible(true);

    Latte::LayoutMenuItemWidget widget(&action, nullptr);
    widget.setIcon(false, QStringLiteral("user-identity"));

    auto *radio = widget.findChild<QRadioButton *>();
    QVERIFY(radio);
    QVERIFY(radio->isVisibleTo(&widget));
    QVERIFY(radio->isChecked());
    QVERIFY(widget.sizeHint().width() > widget.fontMetrics().horizontalAdvance(action.text()));
    QVERIFY(widget.minimumSizeHint().isValid());
}

void ContextMenuUnitTest::layoutMenuItemHidesRadioForNonCheckableAction()
{
    QAction action(QStringLiteral("Plain Layout"), this);
    action.setCheckable(false);
    action.setVisible(true);

    Latte::LayoutMenuItemWidget widget(&action, nullptr);

    auto *radio = widget.findChild<QRadioButton *>();
    QVERIFY(radio);
    QVERIFY(!radio->isVisibleTo(&widget));
    QVERIFY(widget.sizeHint().isValid());
}

QTEST_MAIN(ContextMenuUnitTest)

#include "contextmenuunittest.moc"
