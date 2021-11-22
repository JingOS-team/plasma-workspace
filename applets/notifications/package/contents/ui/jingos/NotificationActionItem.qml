/*
 * Copyright (C) 2021 Beijing Jingling Information System Technology Co., Ltd. All rights reserved.
 *
 * Authors:
 * Liu Bangguo <liubangguo@jingos.com>
 *
 */


import QtQuick 2.12
import QtQuick.Window 2.12
import QtQml 2.12
import QtGraphicalEffects 1.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

import org.kde.plasma.core 2.0 as PlasmaCore
import jingos.display 1.0

Item {
    id: rootActionItem

    property alias hoverEnabled: control.hoverEnabled
    property alias hovered: control.hovered
    property alias applicationName: notifyHead.text
    property alias summary: notifySummary.text
    property alias body: actionBody.text

    property var applicationIconSource

    property NotificationItemStyle qStyle: NotificationItemStyle {}
    property var actionNames: []
    property var actionLabels: []
    property string exitActionType:""

    implicitWidth: control.width
    implicitHeight: control.height
    width: JDisplay.dp(364)
    height: control.implicitHeight

    signal closeClicked
    signal exitFinished(string type)

    function startExit() {
        exitAnim.start()
    }

    onCloseClicked: startExit()

    Control {
        id: control

        width: JDisplay.dp(364)
        height: implicitHeight
        hoverEnabled: true

        x: 0
        y: 0

        padding: JDisplay.dp(16)
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

        NumberAnimation {
            id: enterAnim

            target: control
            running: true
            property: "y"
            from: -control.height - JDisplay.dp(16)
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
            property: "y"
            from: 0
            to: -control.height - JDisplay.dp(16)
            duration: qStyle.animationTime
            easing.type: Easing.OutSine
            onFinished: rootActionItem.exitFinished(exitActionType)
        }

        background: Rectangle {
            id: background

            radius: JDisplay.dp(14)
            color: schemeSource.data["weather"]["isDarkScheme"] ? Qt.rgba(0x26 / 0xFF, 0x26 / 0xFF, 0x2A / 0xFF, 0.90) : Qt.rgba(1, 1, 1, 0.95)
            layer.enabled: qStyle.backgroundShadowEnabled

            layer.effect: DropShadow {
                color: schemeSource.data["weather"]["isDarkScheme"] ? Qt.rgba(1, 1, 242 / 255, 0.3) : Qt.rgba(0, 0, 0, 0.3)
                radius: JDisplay.dp(5)
                samples: qStyle.backgroundShadowSamples
                transparentBorder: qStyle.backgroundShadowBorder
                horizontalOffset: qStyle.backgroundShadowHOffset
                verticalOffset: qStyle.backgroundShadowVOffset
            }
        }

        contentItem: ColumnLayout {

            spacing: JDisplay.dp(10)

            RowLayout {
                spacing: JDisplay.dp(7)
                Layout.fillWidth: true

                Item {
                    id: appImgIcon

                    Layout.preferredWidth: JDisplay.dp(18)
                    Layout.preferredHeight: JDisplay.dp(18)
                    PlasmaCore.IconItem {
                        id: notifyHeadIcon

                        width: JDisplay.dp(18)
                        height: JDisplay.dp(18)
                        source: applicationIconSource
                        usesPlasmaTheme: false
                    }
                }

                Label {
                    id: notifyHead

                    text: ""
                    Layout.fillWidth: true
                    font.pixelSize: JDisplay.sp(13)
                    color: schemeSource.data["weather"]["isDarkScheme"] ? Qt.rgba(1, 1, 1, 0.90) : Qt.rgba(0, 0, 0, 0.90)
                    elide: Text.ElideRight
                }

                Image {
                    id: closeAction

                    Layout.alignment: Qt.AlignLeft
                    Layout.preferredWidth: JDisplay.dp(18)
                    Layout.preferredHeight: JDisplay.dp(18)
                    visible: false
                    source: "./ic_close.svg"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: rootActionItem.closeClicked()
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Label {
                    id: notifySummary

                    Layout.fillWidth: true
                    text: ""
                    font.pixelSize: JDisplay.sp(13)
                    color: schemeSource.data["weather"]["isDarkScheme"] ? Qt.rgba(1, 1, 1, 1) : Qt.rgba(0, 0, 0, 1)
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    font.bold: true
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Label {
                    id: actionBody

                    Layout.fillWidth: true
                    text: ""
                    font.pixelSize: JDisplay.sp(11)
                    color: schemeSource.data["weather"]["isDarkScheme"] ? Qt.rgba(1, 1, 1, 1) : Qt.rgba(0, 0, 0, 1)
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                    maximumLineCount: 4
                }
            }

            Repeater {
                id: repeater

                model: (actionNames.length % 2) ? (parseInt(actionNames.length / 2) +1 ) : parseInt(actionNames.length / 2)

                RowLayout {
                    id: rowLayput
                    property int hereIndex : index

                    Layout.fillWidth: true
                    spacing: JDisplay.dp(14)

                    Repeater {
                        model: {
                            var buttons = [];
                            var actionNames = (rootActionItem.actionNames || []);
                            var actionLabels = (rootActionItem.actionLabels || []);

                            for (var i = 0 + rowLayput.hereIndex * 2; i < rowLayput.hereIndex * 2 + Math.min(actionNames.length - rowLayput.hereIndex * 2, 2) ; i++ ) {
                                buttons.push ({
                                    actionName: actionNames[i],
                                    label: actionLabels[i]
                                });
                            }
                            return buttons;
                        }

                        JingButton {
                            Layout.fillWidth: true
                            height: JDisplay.dp(29)
                            bgcolor: index === 0 && hereIndex === 0 ? "#39C17B" : "#3C4BE8"
                            text: modelData.label || ""
                            fontSize: JDisplay.sp(10)
                            onClicked: {
                                startExit();
                                exitActionType = modelData.actionName
                            }
                        }
                    }
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        propagateComposedEvents: true

        onEntered: {
            closeAction.visible = true
        }

        onPressed: {
            if (mouse.source !== Qt.MouseEventNotSynthesized) {
                closeAction.visible = false
            }
        }

        onExited: {
            closeAction.visible = false
        }

        onCanceled: {
            closeAction.visible = false
        }
    }
}
