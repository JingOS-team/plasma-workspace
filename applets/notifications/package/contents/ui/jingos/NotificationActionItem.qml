/*
 * Copyright 2021 Bob Wu <pengbo.wu@jingos.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
 
import QtQuick 2.12
import QtQuick.Window 2.12
import QtQml 2.12
import QtGraphicalEffects 1.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

import org.kde.plasma.core 2.0 as PlasmaCore

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
    width: Screen.width / 888 * 364
    height: control.implicitHeight

    signal closeClicked
    signal exitFinished(string type )

    function startExit() {
        exitAnim.start()
    }

    onCloseClicked: startExit()

    Control {
        id: control

        width: Screen.width / 888 * 364
        height: implicitHeight
        hoverEnabled: true

        x: 0
        y: 0

        NumberAnimation {
            id: enterAnim

            target: control
            running: true
            property: "y"
            from: -control.height - 16
            to: 0
            duration: qStyle.animationTime
            easing.type: Easing.InQuad
        }

        NumberAnimation {
            id: exitAnim

            target: control
            running: false
            property: "y"
            from: 0
            to: -control.height - 16
            duration: qStyle.animationTime
            easing.type: Easing.InQuad
            onFinished: rootActionItem.exitFinished(exitActionType)
        }

        background: Rectangle {
            radius: 15
            color: Qt.rgba(0, 0, 0, 0.40)
            layer.enabled: qStyle.backgroundShadowEnabled

            layer.effect: DropShadow {
                radius: qStyle.backgroundShadowRadius
                samples: qStyle.backgroundShadowSamples
                transparentBorder: qStyle.backgroundShadowBorder
                horizontalOffset: qStyle.backgroundShadowHOffset
                verticalOffset: qStyle.backgroundShadowVOffset
            }
        }

        topPadding: 15
        bottomPadding: 15
        leftPadding: 20
        rightPadding: 20     
        
        contentItem: ColumnLayout {

            RowLayout {
                spacing: 10
                Layout.fillWidth: true

                Item {
                    id: appImgIcon
                    Layout.preferredWidth: 32
                    Layout.preferredHeight:32
                    PlasmaCore.IconItem {
                        id: notifyHeadIcon
                        width: 32
                        height: 32
                        source: applicationIconSource
                        usesPlasmaTheme: false
                    }
                }
                
                Label {
                    id: notifyHead
                    text: ""
                    Layout.fillWidth: true
                    font.pixelSize: 14
                    font.weight: Font.Thin
                    color: Qt.rgba(1, 1, 1, 0.90)
                    elide: Text.ElideRight
                }

                Image {
                    id: closeAction
                    Layout.alignment: Qt.AlignLeft
                    Layout.preferredWidth: 20
                    Layout.preferredHeight: 20
                    visible: control.hovered
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
                    font.pixelSize: 14
                    color: Qt.rgba(1, 1, 1, 1)
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
                    font.pixelSize: 14
                    color: Qt.rgba(1, 1, 1, 1)
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                    font.bold: true
                }
            }

            Repeater {
                id: repeater

                model: (actionNames.length % 2) ? (parseInt(actionNames.length / 2) +1 ) : parseInt(actionNames.length / 2)

                RowLayout {
                    id: rowLayput
                    property int hereIndex : index

                    Layout.fillWidth: true

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
                            height: 29
                            bgcolor: index === 0 && hereIndex === 0 ? "#39C17B" : "#3C4BE8"  
                            text: modelData.label || ""
                            fontSize: 10
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
}
