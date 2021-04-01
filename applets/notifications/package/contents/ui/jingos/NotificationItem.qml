import QtQuick 2.12
import QtGraphicalEffects 1.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

import org.kde.plasma.core 2.0 as PlasmaCore

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

    onCloseClicked: startExit()

    property alias hoverEnabled: control.hoverEnabled
    property alias hovered: control.hovered

    property NotificationItemStyle qStyle: NotificationItemStyle {}
    property alias applicationName: notifyHead.text
    property alias time: notifyTime.text
    property alias summary: notifySummary.text
    property alias body: notifyText.text
    property alias applicationIconSource: notifyHeadIcon.source

    Control {
        id: control
        implicitWidth: qStyle.width
        implicitHeight: qStyle.minHeight
        width: implicitWidth
        height: implicitHeight
        hoverEnabled: true
        x: 0

        NumberAnimation {
            id: enterAnim
            target: control
            running: true
            property: "x"
            from: -control.width - 16
            to: 0
            duration: qStyle.animationTime
            easing.type: Easing.InQuad
        }
        NumberAnimation {
            id: exitAnim
            target: control
            running: false
            property: "x"
            from: 0
            to: -control.width - 16
            duration: qStyle.animationTime
            easing.type: Easing.InQuad
            onFinished: root.exitFinished()
        }

        background: Rectangle {
            radius: qStyle.backgroundRadius
            color: qStyle.backgroundColor
            layer.enabled: qStyle.backgroundShadowEnabled
            layer.effect: DropShadow {
                radius: qStyle.backgroundShadowRadius
                samples: qStyle.backgroundShadowSamples
                color: qStyle.backgroundShadowColor
                transparentBorder: qStyle.backgroundShadowBorder
                horizontalOffset: qStyle.backgroundShadowHOffset
                verticalOffset: qStyle.backgroundShadowVOffset
            }
        }
        padding: qStyle.padding
        contentItem: GridLayout {
            id: contentLayout
            columnSpacing: 0
            rowSpacing: qStyle.space
            width: control.availableWidth
            onImplicitHeightChanged: {
                control.height = Math.max(qStyle.minHeight,
                                          implicitHeight + control.padding * 2)
                control.implicitHeight = control.height
            }
            columns: 3
            Item {
                width: qStyle.iconSize
                height: qStyle.iconSize
                PlasmaCore.IconItem {
                    id: notifyHeadIcon
                    width: qStyle.iconSize
                    height: qStyle.iconSize
                    source: "start-here-kde-plasma"
                    usesPlasmaTheme: false
                    onSourceChanged: console.log(this, source)
                }
            }
            Label {
                id: notifyHead
                text: "Mail"
                Layout.fillWidth: true
                font: qStyle.headerFont
                color: qStyle.headerColor
                Layout.leftMargin: qStyle.headerLeftMargin
            }
            Text {
                id: notifyTime
                Layout.preferredHeight: qStyle.actionIconSize
                visible: !control.hovered
                verticalAlignment: Text.AlignVCenter
                text: Qt.formatTime(new Date(), "hh:mm:ss")
                font: qStyle.timeFont
                color: qStyle.timeColor
            }
            Image {
                id: closeAction
                visible: control.hovered
                Layout.preferredWidth: qStyle.actionIconSize
                Layout.preferredHeight: qStyle.actionIconSize
                source: "./ic_close.svg"
                MouseArea {
                    anchors.fill: parent
                    onClicked: root.closeClicked()
                }
            }
            Label {
                id: notifySummary
                Layout.preferredWidth: parent.width
                Layout.preferredHeight: contentHeight
                text: "sss SSS SSS SSS SSS SSS SSS SSS SSS SSS SSS"
                Layout.columnSpan: 3
                visible: text && text !== ""
                wrapMode: Text.Wrap
                font: qStyle.summaryFont
                color: qStyle.summaryColor
            }
            Label {
                id: notifyText
                Layout.preferredWidth: parent.width
                Layout.preferredHeight: contentHeight
                text: "texttttt 11111111 1111111111 fffffffffffff fffffffff fffffff"
                Layout.columnSpan: 3
                wrapMode: Text.Wrap
                visible: text && text !== ""
                font: qStyle.contentFont
                color: qStyle.contentColor
            }
        }
    }
}
