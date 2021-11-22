/*
 * Copyright (C) 2021 Beijing Jingling Information System Technology Co., Ltd. All rights reserved.
 *
 * Authors:
 * Liu Bangguo <liubangguo@jingos.com>
 *
 */
import QtQuick 2.0
import jingos.display 1.0

QtObject {
    property int width: JDisplay.dp(240)
    property int minHeight: JDisplay.dp(97)
    property int padding: JDisplay.dp(10)
    property int space: JDisplay.dp(0)
    property int iconSize: JDisplay.dp(13)
    property int actionIconSize: JDisplay.dp(22)

    property color backgroundColor: "#FFFFFFFF"
    property color backgroundColorBlack: "#CC26262A"

    property int backgroundRadius: JDisplay.dp(9)
    property bool backgroundShadowEnabled: false
    property color backgroundShadowColor: "#F0A0A0A0"
    property int backgroundShadowRadius: JDisplay.dp(16)
    property int backgroundShadowSamples: JDisplay.dp(33)
    property bool backgroundShadowBorder: true
    property int backgroundShadowHOffset: JDisplay.dp(0)
    property int backgroundShadowVOffset: JDisplay.dp(0)

    property int headerLeftMargin: JDisplay.dp(8)

    property color headerColor: "#99000000"
    property color headerColorBlack: "#8CF7F7F7"
    property color timeColor: "#99000000"
    property color timeColorBlack: "#8CF7F7F7"

    property color summaryColor: "#FF000000"
    property color summaryColorBlack: "#FFF7F7F7"
    property color contentColor: "#FF000000"
    property color contentColorBlack: "#FFF7F7F7"
    property int animationTime: 75
}
