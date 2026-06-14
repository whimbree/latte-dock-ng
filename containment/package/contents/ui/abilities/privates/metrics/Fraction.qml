/*
    SPDX-FileCopyrightText: 2020 Michail Vourlakos <mvourlakos@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.0

import "../../../definition/metrics" as MetricsDef

Item {
    // Default visual-proportion values; overridden by MetricsPrivate bindings.
    // See MetricsDef.Constants for the authoritative named values.
    property real thicknessMargin: MetricsDef.Constants.kDefaultLengthPadding
    property real lengthMargin: MetricsDef.Constants.kDefaultLengthPadding
    property real lengthPadding: MetricsDef.Constants.kDefaultLengthPadding
    property real lengthAppletPadding: MetricsDef.Constants.kDefaultLengthPadding
}
