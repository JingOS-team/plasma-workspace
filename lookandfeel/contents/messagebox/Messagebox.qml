/*
 * Copyright (C) 2021 Beijing Jingling Information System Technology Co., Ltd. All rights reserved.
 *
 * Authors:
 * Liu Bangguo <liubangguo@jingos.com>
 *
 */
import QtQuick 2.15
import QtQuick.Window 2.2
import QtQuick.Controls 2.14
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.plasma.private.nanoshell 2.0 as NanoShell
import org.kde.plasma.extras 2.0 as PlasmaExtra
import org.kde.kirigami 2.15 //as Kirigami
import QtGraphicalEffects 1.6
import jingos.display 1.0

NanoShell.FullScreenOverlay {
    id: window

    visible: false
    property string title: "title"
    property string content: "content"
    property bool isDark: JTheme.colorScheme == "jingosDark"
    property color titleColor: JTheme.majorForeground  //"#000000"
    property color textColor: JTheme.majorForeground  //"#000000"
    property color centerButtonTextColor : JTheme.buttonWeakForeground  //"#3C4BE8"

    color: "transparent"

    Rectangle {
        id: contentArea
        z: 99
        radius:JDisplay.dp(10)
        visible: true

        width: JDisplay.dp(231)
        height: clayout.height
        color: isDark ? Qt.rgba(38 / 255,38 / 255,42 / 255,0.9) : Qt.rgba(1,1,1,1)
        anchors.centerIn: parent

        Column{
            id:clayout
            width: parent.width
            bottomPadding : JDisplay.dp(19)
            leftPadding : JDisplay.dp(16)
            rightPadding : JDisplay.dp(16)
            topPadding : JDisplay.dp(16)

            Item {
                height: titleText.height + JDisplay.dp(10)
                width: parent.width - JDisplay.dp(32)
                Text {
                    id: titleText
                    width: parent.width

                    horizontalAlignment: Text.AlignHCenter
                    color: window.titleColor
                    wrapMode: Text.WordWrap
                    font.pixelSize: JDisplay.sp(16)
                    font.weight: Font.Medium
                    text: i18nd("plasma-phone-components", window.title)
                }
            }

            Item {
                height: textText.height + JDisplay.dp(20)
                width: parent.width - JDisplay.dp(32)
                Text {
                    id: textText
                    width: parent.width

                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    color: window.textColor
                    font.pixelSize: JDisplay.sp(12)
                    text: i18nd("plasma-phone-components", window.content)
                }
            }

            Item {
                height:oneButton.height
                width: parent.width - JDisplay.dp(32)
                JButton{
                    id: oneButton
                    anchors.left : parent.left

                    width:parent.width
                    height: JDisplay.dp(36)
                    radius: JDisplay.dp(7)
                    backgroundColor: JTheme.buttonPopupBackground

                    fontColor: window.centerButtonTextColor
                    font.pointSize: JDisplay.sp(11)
                    text: i18nd("plasma_shell_org.kde.plasma.phone", "OK")

                    onClicked: {
                        window.close()
                    }
                }
            }

        }
    }

    DropShadow {
        anchors.fill: contentArea
        horizontalOffset: 0
        verticalOffset: JDisplay.dp(1)
        radius: 12.0
        samples: 24
        cached: true
        color: Qt.rgba(0, 0, 0, 0.1)
        source: contentArea
    }
}


