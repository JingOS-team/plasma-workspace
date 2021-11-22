/*
 * Copyright (C) 2021 Beijing Jingling Information System Technology Co., Ltd. All rights reserved.
 *
 * Authors:
 * Liu Bangguo <liubangguo@jingos.com>
 *
 */
import QtQuick 2.12
import QtGraphicalEffects 1.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.core 2.0
import org.kde.plasma.private.digitalclock 1.0 as DC
import jingos.display 1.0
Item {
    id: root

    implicitWidth: control.width
    implicitHeight: control.height
    width: implicitWidth
    height: implicitHeight
    signal closeClicked
    signal exitFinished

    function startExit() {
        exitAnim.start()
    }

    function formatTimeString(formatTime) {
        var timeStr = Qt.formatTime(formatTime, timezoneProxy.isSystem24HourFormat ? "h:mm" : "h:mm AP");
        if (timeStr.search("上午") != -1 || timeStr.search("下午") != -1) {
            var arr = timeStr.split(" ")
            timeStr = arr[1] + " " + arr[0]
        }
        return timeStr;
    }

    onCloseClicked: {
        instantiator.deleteAll()
    }

    property alias hoverEnabled: control.hoverEnabled
    property alias hovered: control.hovered
    property alias applicationName: notifyHead.text
    property alias time: notifyTime.text
    property alias summary: notifySummary.text
    property alias body: notifyText.text
    property alias applicationIconSource: notifyHeadIcon.source
    property NotificationItemStyle qStyle: NotificationItemStyle {}

    Control {
        id: control

        implicitWidth: qStyle.width
        implicitHeight: qStyle.minHeight
        width: implicitWidth
        height: implicitHeight
        hoverEnabled: true
        x: 0

        onHoveredChanged: {
            if (hovered) {
                instantiator.hoverAll(true)
            } else {
                instantiator.hoverAll(false)
            }
        }

        NumberAnimation {
            id: enterAnim

            target: control
            running: true
            property: "x"
            from: -control.width - 16
            to: 0
            duration: qStyle.animationTime
            easing.type: Easing.OutSine

            onFinished: {
                background.layer.enabled = true
            }
        }

        NumberAnimation {
            id: exitAnim

            target: control
            running: false
            property: "x"
            from: control.x
            to: -control.width - 16
            duration: qStyle.animationTime
            easing.type: Easing.OutSine
            onFinished: root.exitFinished()
        }

        background: Rectangle {
            id: background

            radius: qStyle.backgroundRadius
            color: schemeSource.data["weather"]["isDarkScheme"] ? qStyle.backgroundColorBlack : qStyle.backgroundColor
            layer.enabled: qStyle.backgroundShadowEnabled
            layer.effect: DropShadow {
                radius: qStyle.backgroundShadowRadius
                samples: qStyle.backgroundShadowSamples
                color: schemeSource.data["weather"]["isDarkScheme"] ? "#000000" : qStyle.backgroundShadowColor
                transparentBorder: qStyle.backgroundShadowBorder
                horizontalOffset: qStyle.backgroundShadowHOffset
                verticalOffset: qStyle.backgroundShadowVOffset
            }
        }

        padding: qStyle.padding
        contentItem: GridLayout {
            id: contentLayout

            columnSpacing: qStyle.headerLeftMargin
            rowSpacing: qStyle.space
            width: control.availableWidth
            onImplicitHeightChanged: {
                control.height = Math.max(qStyle.minHeight -  control.padding * 2, implicitHeight + control.padding * 2  )
                control.implicitHeight = control.height
            }
            columns: 3

            PlasmaCore.IconItem {
                id: notifyHeadIcon

                Layout.maximumWidth: qStyle.iconSize
                Layout.minimumWidth: qStyle.iconSize
                Layout.maximumHeight: qStyle.iconSize
                Layout.minimumHeight: qStyle.iconSize
                Layout.fillWidth: true

                source: "start-here-kde-plasma"
                visible: source !== ""
                usesPlasmaTheme: false
            }

            Label {
                id: notifyHead

                text: "Mail"
                Layout.fillWidth: true
                Layout.minimumWidth: contentWidth
                Layout.minimumHeight: contentHeight
                font.pixelSize: JDisplay.sp(11)
                color: schemeSource.data["weather"]["isDarkScheme"] ? qStyle.headerColorBlack : qStyle.headerColor

                elide: Text.ElideRight

                Layout.columnSpan: {
                    if (notifyHeadIcon.visible) {
                        return 1
                    } else {
                        return 2
                    }
                }
            }

            Text {
                id: notifyTime

                Layout.preferredWidth: contentWidth
                Layout.preferredHeight: qStyle.actionIconSize
                Layout.fillWidth: false

                visible: !control.hovered
                Layout.alignment: Qt.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: ""
                font.pixelSize: JDisplay.sp(10)
                color: schemeSource.data["weather"]["isDarkScheme"] ? qStyle.timeColorBlack : qStyle.timeColor
            }

            PlasmaCore.DataSource {
                id: schemeSource

                engine: "weather"
                connectedSources: ["weather"]
                onSourceAdded: {
                    if (source === "weather") {
                        disconnectSource(source);
                        connectSource(source);
                    }
                }
            }

            Image {
                id: closeAction

                Layout.preferredWidth: qStyle.actionIconSize
                Layout.preferredHeight: qStyle.actionIconSize

                visible: control.hovered
                source: "./ic_close.svg"

                MouseArea {
                    width: parent.width * 2
                    height: parent.height * 2
                    anchors.centerIn: parent
                    onClicked: root.closeClicked()
                }
            }

            Label {
                id: notifySummary

                Layout.preferredWidth: parent.width
                Layout.preferredHeight: contentHeight
                Layout.columnSpan: 3

                text: ""
                wrapMode: Text.WrapAnywhere
                font.pixelSize: JDisplay.sp(11)
                font.bold: true
                color: schemeSource.data["weather"]["isDarkScheme"] ? qStyle.summaryColorBlack: qStyle.summaryColor
            }

            Label {
                id: notifyText

                Layout.preferredWidth: parent.width
                Layout.preferredHeight: implicitHeight
                Layout.columnSpan: 3
                height: implicitHeight
                width: parent.width
                maximumLineCount: 4

                text: ""
                font.pixelSize:  JDisplay.sp(11)
                color: schemeSource.data["weather"]["isDarkScheme"] ? qStyle.contentColorBlack : qStyle.contentColor
                wrapMode: Text.WrapAnywhere
                elide: Text.ElideRight
            }
        }
        MouseArea {
            id: dragMouseArea

            anchors.fill: parent
            drag.target: parent
            drag.axis: Drag.XAxis
            drag.minimumX: -parent.width
            drag.maximumX: 0
            propagateComposedEvents: true
            hoverEnabled: true

            onReleased: {
                instantiator.hoverAll(false)
                if (control.x < 0) {
                    root.closeClicked()
                }
            }
            onPressed: {
                instantiator.hoverAll(true)
            }
        }
    }

    DataSource {
        id: timeSource

        engine: "time"
        connectedSources: ["Local"]
        interval: 1000
    }

    DC.TimeZoneFilterProxy {
        id: timezoneProxy
    }
}
