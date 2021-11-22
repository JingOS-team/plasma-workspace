/*
   Copyright (c) 2015 Marco Martin <mart@kde.org>
   Copyright (c) 2021 Liu Bangguo <liubangguo@jingos.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

import QtQuick 2.7
import QtQuick.Window 2.2 // for Screen
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2 as QtControls
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.1 as QtDialogs
import org.kde.kirigami 2.15 as Kirigami
import org.kde.newstuff 1.62 as NewStuff
import org.kde.kcm 1.3 as KCM
import jingos.display 1.0

import org.kde.private.kcm_cursortheme 1.0
 Rectangle {
    id:root

    property int statusbar_height : JDisplay.dp(22)
    property int statusbar_icon_size: JDisplay.dp(22)
    property int default_setting_item_height: JDisplay.dp(45)
    property int default_inner_title_height: JDisplay.dp(30)

    property int marginTitle2Top : JDisplay.dp(48)
    property int marginItem2Title : JDisplay.dp(36)
    property int marginLeftAndRight : JDisplay.dp(20)
    property int marginItem2Top : JDisplay.dp(24)
    property int radiusCommon: JDisplay.dp(10)
    property int fontNormal: JDisplay.sp(14)

    property var selsectType: "" //jing_cursor breeze_cursors 
    property bool isChanged: false

    width: parent.width
    height: parent.height
    color: Kirigami.JTheme.settingMinorBackground//"#FFF6F9FF"
    property bool isDarkTheme: Kirigami.JTheme.colorScheme === "jingosDark"

    Component.onCompleted:{
        selsectType = kcm.cursorThemeSettings.cursorTheme
    }

    Text {
        id: title

        anchors {
            left: parent.left
            top: parent.top
            leftMargin: marginLeftAndRight  
            topMargin: marginTitle2Top  
        }
        height: JDisplay.dp(14)
        width: JDisplay.dp(329)
        verticalAlignment:Text.AlignVCenter
        text: i18n("Pointer Control")
        font.pixelSize: JDisplay.sp(20)
        font.weight: Font.Bold
        color: Kirigami.JTheme.majorForeground
    }

    Kirigami.JIconButton {
        id: confirmIcon
        
        anchors{
            top: title.top
            topMargin: -(confirmIcon.height - title.height) / 2
            right: parent.right
            rightMargin: JDisplay.dp(20)
        }

        width: applyText.width  + JDisplay.dp(10)
        height: applyText.height + JDisplay.dp(10)

        enabled: isChanged

        Text{
            id:applyText

            anchors.centerIn: parent
            anchors.leftMargin:JDisplay.dp(10)
            verticalAlignment:Text.AlignVCenter
            
            height:JDisplay.dp(21)

            font.pixelSize: JDisplay.sp(17)
            text: i18n("Apply")
            color: confirmIcon.enabled ? Kirigami.JTheme.highlightColor : isDarkTheme ? "#4DF7F7F7" : "#4D000000"
        }

        onClicked: {
            saveDialog.visible = true
        }
    }

    Rectangle {
        id: brightness_area

        anchors {
            left: parent.left
            top: title.bottom
            leftMargin: marginLeftAndRight
            topMargin: marginItem2Title
        }

        width: parent.width - marginLeftAndRight * 2
        height: default_setting_item_height + default_inner_title_height
        color: Kirigami.JTheme.cardBackground//"#fff"
        radius: 10

        Rectangle {

            id: brightness_title_item

            width: parent.width
            height: default_inner_title_height
            radius:10 
            color: "transparent"

            Text {
                id: brightness_title

                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                    leftMargin: marginLeftAndRight
                }
                width: JDisplay.dp(329)
                height: JDisplay.dp(14)
                text: i18n("Pointer Size")
                font.pixelSize: JDisplay.sp(12)
                color: Kirigami.JTheme.minorForeground//"#4D000000"
            }
        }

        Rectangle {
            id: brightness_top

            anchors.top:brightness_title_item.bottom
            width: parent.width
            height: default_setting_item_height
            color: "transparent"
                
            Text {
                id: ic_small

                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                    leftMargin: marginLeftAndRight
                }
                text: i18n("Small")
                font.pixelSize: JDisplay.sp(14)
                color:Kirigami.JTheme.minorForeground//"#99000000"
            }

            Text {
                id: ic_large

                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    rightMargin: marginLeftAndRight
                }
                text: i18n("Big")
                font.pixelSize: JDisplay.sp(14)
                color:Kirigami.JTheme.minorForeground//"#99000000"
            }

            Kirigami.JSlider {
                id: brightness_slider

                anchors {
                    verticalCenter: parent.verticalCenter
                    left: ic_small.right
                    right: ic_large.left
                    leftMargin: JDisplay.dp(17)
                    rightMargin: JDisplay.dp(17)
                }

                value: kcm.cursorThemeSettings.cursorSize
                from: 24
                to: 48
                stepSize: 12
                onValueChanged: {
                    kcm.cursorThemeSettings.cursorSize = value
                }
                
                onMoved:{
                    isChanged = true
                }
            }
        }
    }

    Rectangle {
        id: sleep_area

        anchors {
            left: parent.left
            top: brightness_area.bottom
            leftMargin: marginLeftAndRight
            topMargin: marginItem2Top
        }

        width: parent.width - marginLeftAndRight* 2
        height: JDisplay.dp(210)
        color: Kirigami.JTheme.cardBackground//"#fff"
        radius: radiusCommon

        Text {
            id: styleText

            anchors {
                top: parent.top
                left: parent.left
                leftMargin: JDisplay.dp(20)
                topMargin: JDisplay.dp(12)
            }

            text: i18n("Pointer style")
            font.pixelSize: fontNormal
            color:Kirigami.JTheme.minorForeground//"#4D000000"
        }
        Item {
            anchors{
                top: styleText.bottom
                topMargin: JDisplay.dp(12)
            }

            width:parent.width
            height: JDisplay.dp(146)

            Item {
                id: circleItem
                width: parent.width / 2
                height: parent.height

                Item{
                    anchors.horizontalCenter: parent.horizontalCenter

                    width: JDisplay.dp(130)
                    height:childrenRect.height

                    Rectangle{
                        id: circleRect

                        anchors.horizontalCenter: parent.horizontalCenter

                        width: JDisplay.dp(130)
                        height: JDisplay.dp(95)

                        radius:10 
                        color:Kirigami.JTheme.dividerForeground//"#14747480"

                        Item{
                            anchors.horizontalCenter:parent.horizontalCenter
                            anchors.verticalCenter:parent.verticalCenter
                            
                            width: childrenRect.width
                            height: JDisplay.dp(35)

                            Image{
                                id:img_circle

                                anchors.verticalCenter:parent.verticalCenter

                                width: JDisplay.dp(33)
                                height: JDisplay.dp(33)

                                source:"../image/icon_circle.png"
                            }
                            Image{
                                anchors{
                                    left:img_circle.right
                                    leftMargin: 7
                                    verticalCenter:parent.verticalCenter
                                }

                                width: JDisplay.dp(15)
                                height: JDisplay.dp(35)
                                
                                source:"../image/icon_rectangle.png"
                            }

                        }
                    }

                    Text{
                        id: circleText

                        anchors{
                            top: circleRect.bottom
                            topMargin: JDisplay.dp(10)
                            horizontalCenter: parent.horizontalCenter
                        }

                        text:i18n("Circular style")
                        font.pixelSize:fontNormal
                        color:Kirigami.JTheme.majorForeground//"black"
                    }

                    QtControls.CheckBox{
                        id: cb_circle
                        anchors{
                            top: circleText.bottom
                            topMargin: 7
                            horizontalCenter: parent.horizontalCenter
                        }
                        checked:selsectType == "jing_cursor"

                        indicator: Rectangle {
                            implicitWidth: JDisplay.dp(17)
                            implicitHeight: JDisplay.dp(17)
                            border.color: cb_circle.checked ? Kirigami.JTheme.highlightColor:Kirigami.JTheme.dividerForeground//"#FF3C4BE8" : "#FFC7C7C7"
                            color: cb_circle.checked ? Kirigami.JTheme.highlightColor : "transparent"//"#FF3C4BE8" : "white"
                            border.width: 1
                            radius:4

                            Image{
                                visible:cb_circle.checked
                                width:parent.width
                                height:parent.height
                                source:"../image/icon_select.png"
                            }
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked:{
                            isChanged = true
                            selsectType = "jing_cursor"
                            kcm.cursorThemeSettings.cursorTheme = kcm.cursorThemeFromIndex(2)
                        }
                    }
                }

            }
            Item {
                id: arrowItem
                anchors.left: circleItem.right
                width: parent.width / 2
                height: parent.height
                Item{
                    anchors.horizontalCenter: parent.horizontalCenter

                    width: JDisplay.dp(130 )
                    height:childrenRect.height
                    Rectangle{
                        id: arrowRect

                        anchors.horizontalCenter: parent.horizontalCenter

                        width: JDisplay.dp(130)
                        height: JDisplay.dp(95)

                        radius:10 
                        color:Kirigami.JTheme.dividerForeground//"#14747480"

                        Item{
                            anchors.horizontalCenter:parent.horizontalCenter
                            anchors.verticalCenter:parent.verticalCenter
                            
                            width: childrenRect.width
                            height: JDisplay.dp(35)

                            Image{
                                id:img_arrow

                                anchors.verticalCenter:parent.verticalCenter

                                width: JDisplay.dp(24)
                                height: JDisplay.dp(26)

                                source:"../image/icon_arrow.png"
                            }

                            Image{
                                anchors{
                                    left:img_arrow.right
                                    leftMargin: JDisplay.dp(13)
                                    verticalCenter:parent.verticalCenter
                                }

                                width: JDisplay.dp(15)
                                height: JDisplay.dp(35)
                                
                                source:"../image/icon_rectangle.png"
                            }

                        }
                    }

                    Text{
                        id: arrowText

                        anchors{
                            top: arrowRect.bottom
                            topMargin: JDisplay.dp(10)
                            horizontalCenter: parent.horizontalCenter
                        }

                        text:i18n("Arrow style")
                        font.pixelSize:fontNormal
                        color:Kirigami.JTheme.majorForeground//"black"
                    }

                    QtControls.CheckBox{
                        id:cb_arrow

                        anchors{
                            top: arrowText.bottom
                            topMargin: JDisplay.dp(7)
                            horizontalCenter: parent.horizontalCenter
                        }

                        checked: selsectType == "breeze_cursors"

                        indicator: Rectangle {
                            implicitWidth: JDisplay.dp(17)
                            implicitHeight: JDisplay.dp(17)
                            border.color: cb_arrow.checked ? Kirigami.JTheme.highlightColor:Kirigami.JTheme.dividerForeground//"#FF3C4BE8" : "#FFC7C7C7"
                            color: cb_arrow.checked ? Kirigami.JTheme.highlightColor : "transparent"//"#FF3C4BE8" : "white"
                            border.width: 1
                            radius:4

                            Image{
                                width:parent.width
                                height:parent.height

                                visible:cb_arrow.checked
                                source:"../image/icon_select.png"
                            }
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked:{
                            isChanged = true
                            selsectType = "breeze_cursors"
                            kcm.cursorThemeSettings.cursorTheme = kcm.cursorThemeFromIndex(0)
                        }
                    }
                }
            }

        }
    }

    Kirigami.JDialog {
        id: saveDialog

        title: i18n("Restart")
        text: i18n("Applying this setting will restart your PAD")
        leftButtonText: i18n("Cancel")
        rightButtonText: i18n("Restart")
        dim: true
        focus: true

        onLeftButtonClicked: {
            saveDialog.visible = false
        }

        onRightButtonClicked: {
            kcm.save()
            saveDialog.visible = false
            kcm.rebootDevice()
        }
    }
}
/*KCM.GridViewKCM {
    id: root
    KCM.ConfigModule.quickHelp: i18n("This module lets you choose the mouse cursor theme.")

    view.model: kcm.cursorsModel
    view.delegate: Delegate {}
    view.currentIndex: kcm.cursorThemeIndex(kcm.cursorThemeSettings.cursorTheme);

    view.onCurrentIndexChanged: {
        kcm.cursorThemeSettings.cursorTheme = kcm.cursorThemeFromIndex(view.currentIndex)
        view.positionViewAtIndex(view.currentIndex, view.GridView.Beginning);
    }

    Component.onCompleted: {
        view.positionViewAtIndex(view.currentIndex, GridView.Beginning);
    }

    KCM.SettingStateBinding {
        configObject: kcm.cursorThemeSettings
        settingName: "cursorTheme"
        extraEnabledConditions: !kcm.downloadingFile
    }

    DropArea {
        anchors.fill: parent
        onEntered: {
            if (!drag.hasUrls) {
                drag.accepted = false;
            }
        }
        onDropped: kcm.installThemeFromFile(drop.urls[0])
    }

    footer: ColumnLayout {
        id: footerLayout

        Kirigami.InlineMessage {
            id: infoLabel
            Layout.fillWidth: true

            showCloseButton: true

            Connections {
                target: kcm
                onShowSuccessMessage: {
                    infoLabel.type = Kirigami.MessageType.Positive;
                    infoLabel.text = message;
                    infoLabel.visible = true;
                }
                onShowInfoMessage: {
                    infoLabel.type = Kirigami.MessageType.Information;
                    infoLabel.text = message;
                    infoLabel.visible = true;
                }
                onShowErrorMessage: {
                    infoLabel.type = Kirigami.MessageType.Error;
                    infoLabel.text = message;
                    infoLabel.visible = true;
                }
            }
        }

        RowLayout {
            id: row1
                //spacer
                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

            RowLayout {
                id: comboLayout

                KCM.SettingStateBinding {
                    configObject: kcm.cursorThemeSettings
                    settingName: "cursorSize"
                    extraEnabledConditions: kcm.canResize
                }

                QtControls.Label {
                    //text: i18n("Size:")
                    text: kcm.cursorThemeSettings.cursorTheme
                }
                QtControls.ComboBox {
                    id: sizeCombo
                    model: kcm.sizesModel
                    textRole: "display"
                    currentIndex: kcm.cursorSizeIndex(kcm.cursorThemeSettings.cursorSize);
                    onActivated: {
                        kcm.cursorThemeSettings.cursorSize = kcm.cursorSizeFromIndex(sizeCombo.currentIndex);
                        kcm.preferredSize = kcm.cursorSizeFromIndex(sizeCombo.currentIndex);
                    }

                    delegate: QtControls.ItemDelegate {
                        id: sizeComboDelegate

                        readonly property int size: parseInt(model.display)

                        width: parent.width
                        highlighted: ListView.isCurrentItem
                        text: model.display

                        contentItem: RowLayout {
                            Kirigami.Icon {
                                source: model.decoration
                                smooth: true
                                width: sizeComboDelegate.size / Screen.devicePixelRatio
                                height: sizeComboDelegate.size / Screen.devicePixelRatio
                                visible: valid && sizeComboDelegate.size > 0
                            }

                            QtControls.Label {
                                Layout.fillWidth: true
                                color: sizeComboDelegate.highlighted ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
                                text: model[sizeCombo.textRole]
                                elide: Text.ElideRight
                            }
                        }
                    }
                }
            }
            RowLayout {
                parent: footerLayout.x + footerLayout.width - comboLayout.width > width ? row1 : row2
                QtControls.Button {
                    icon.name: "document-import"
                    text: i18n("&Install from File...")
                    onClicked: fileDialogLoader.active = true;
                    enabled: kcm.canInstall
                }
                NewStuff.Button {
                    id: newStuffButton
                    enabled: kcm.canInstall
                    text: i18n("&Get New Cursors...")
                    configFile: "xcursor.knsrc"
                    viewMode: NewStuff.Page.ViewMode.Tiles
                    onChangedEntriesChanged: kcm.ghnsEntriesChanged(newStuffButton.changedEntries);
                }
            }
        }
        RowLayout {
            id: row2
            visible: children.length > 1
            //spacer
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }

    Loader {
        id: fileDialogLoader
        active: false
        sourceComponent: QtDialogs.FileDialog {
            title: i18n("Open Theme")
            folder: shortcuts.home
            nameFilters: [ i18n("Cursor Theme Files (*.tar.gz *.tar.bz2)") ]
            Component.onCompleted: open()
            onAccepted: {
                kcm.installThemeFromFile(fileUrls[0])
                fileDialogLoader.active = false
            }
            onRejected: {
                fileDialogLoader.active = false
            }
        }
    }
}*/

