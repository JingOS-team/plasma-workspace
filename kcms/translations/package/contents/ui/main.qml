/*
 * Copyright (C) 2015 Marco Martin <mart@kde.org>
 * Copyright (C) 2018 Eike Hein <hein@kde.org>
 * Copyright (C) 2021 Liu Bangguo <liubangguo@jingos.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
*/

import QtQuick 2.1
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.3 as QtControls
import org.kde.kirigami 2.15 as Kirigami
import org.kde.plasma.core 2.1 as PlasmaCore
import org.kde.kcm 1.2
import org.jingos.settings.language 1.0
import jingos.display 1.0

// Kirigami.Page{
Rectangle {
    id: root

    property int screenWidth: 888
    property int screenHeight: 648

    property var appScaleSize: JDisplay.dp(1.0)
    property var appFontSize: JDisplay.sp(1.0)

    property int statusbar_height : 22 * appScaleSize
    property int statusbar_icon_size: 22 * appScaleSize
    property int default_setting_item_height: 45 * appScaleSize
    property int default_setting_title_height: 30 * appScaleSize

    property int marginTitle2Top : 44 * appScaleSize
    property int marginItem2Title : 36 * appScaleSize
    property int marginLeftAndRight : 20 * appScaleSize
    property int marginItem2Top : 24 * appScaleSize
    property int radiusCommon: 10 * appScaleSize
    property int fontNormal: 14

    property string numberStr: i18n("Numbers")
    property string timeStr: i18n("Time")
    property string currencyStr: i18n("Currency")
    property string muStr: i18n("Measurement Units")

    property string numberVStr: regionManager.lcNumericContent
    property string timeVStr: regionManager.lcTimeContent
    property string currencyVStr: regionManager.lcMonetaryContent
    property string muVStr: regionManager.lcMeasurementContent

    property variant regionArray: [numberStr,timeStr,currencyStr,muStr]
    property variant regionValueArray: [numberVStr,timeVStr,currencyVStr,muVStr]
    property bool isUpddateLang: false
    property int regionSelectIndex : -2
    property string updateLanguage: ""

    width: screenWidth * 0.7
    height: screenHeight

    Connections {
        target: kcm

        onCurrentIndexChanged:{
            if(index == 1){
                popAllView()
            }
        }
    }

    function popAllView() {
        while (pageManger.depth > 1) {
            pageManger.pop()
        }
    }

    TranslateTool {
        id: translateTool
    }

    RegionManager {
       id: regionManager
    }

    Component{
        id: mainComponent

        Rectangle{
            width: root.width
            height: root.height
            color: Kirigami.JTheme.settingMinorBackground
            Component {
                id: languagesListItemComponent
                Item {
                    id: listItemdel

                    width: languagesList.width
                    height: 45 * appScaleSize

                    RowLayout {
                        anchors.fill: parent
                        spacing: 10  * appScaleSize

                        QtControls.Label {
                            Layout.fillWidth: true

                            Layout.alignment: Qt.AlignVCenter
                            text:  model.display
                            color: (index == 0) ? Kirigami.JTheme.highlightColor : Kirigami.JTheme.majorForeground
                            font.pixelSize: 16 * appFontSize
                        }

                        Image {
                            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                            source:"../image/menu_select.png"
                            width: 22 * appScaleSize
                            height: 22 * appScaleSize
                            sourceSize.width: 22 * appScaleSize
                            sourceSize.height: 22 * appScaleSize
                            visible: index === 0
                        }
                    }

                    Kirigami.Separator {
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        visible: index !== (languagesList.count - 1)
                    }

                    MouseArea {
                        anchors.fill: parent

                        onClicked: {
                            if (index === 0) {
                                return
                            }
                            updateLanguage = model.LanguageCode
                            kcm.selectedTranslationsModel.move(index, 0)
                            isUpddateLang = true
                        }
                    }
                }
            }
            Text {
                id: title

                anchors {
                    left: parent.left
                    top: parent.top
                    leftMargin: marginLeftAndRight
                    topMargin: marginTitle2Top
                }
                width: 329 * appScaleSize
                height: 14 * appScaleSize

                text: i18n("Language & Region")
                font.pixelSize: 20 * appFontSize
                font.weight: Font.Bold
                color: Kirigami.JTheme.majorForeground
            }

            Kirigami.JButton {
                id: applyTitle

                anchors {
                    right:list_area.right
                    top: parent.top
                    topMargin: marginTitle2Top
                }
                height: 25 * appScaleSize
                width: 60 * appScaleSize

                text: i18n("Apply")
                font.pixelSize: 17 * appFontSize
                backgroundColor: "transparent"
                fontColor: isUpddateLang ? Kirigami.JTheme.highlightColor : Kirigami.JTheme.disableForeground
                MouseArea{
                    width: 40 * appScaleSize
                    height: 40 * appScaleSize
                    enabled: isUpddateLang
                    onClicked: {
                        confirmDlg.open()
                    }
                }
            }

            Rectangle {
                id: list_area

                anchors {
                    top : title.bottom
                    topMargin: marginItem2Title
                    left: parent.left
                    leftMargin: marginLeftAndRight
                    right:parent.right
                    rightMargin: marginLeftAndRight
                }

                width: parent.width- marginLeftAndRight * 2
                height: languagesList.height + 10 * appScaleSize + language_title.height
                color: Kirigami.JTheme.cardBackground
                radius: radiusCommon

                Rectangle {
                    id: language_title

                    anchors {
                        top : parent.top
                        topMargin: 10 * appScaleSize
                        left: parent.left
                        right: parent.right
                    }
                    width: parent.width
                    height: 12 * appScaleSize
                    radius: radiusCommon

                    color: "transparent"
                    Text {
                        anchors {
                            left: language_title.left
                            leftMargin: marginLeftAndRight
                            verticalCenter:parent.verticalCenter
                        }
                        height: 12 * appScaleSize
                        verticalAlignment:Text.AlignVCenter

                        text: i18n("Preferred language order")
                        font.pixelSize: 12 * appFontSize
                        color: Kirigami.JTheme.disableForeground
                    }
                }

                ListView {
                    id: languagesList

                    anchors {
                        top : language_title.bottom
                        left: parent.left
                        leftMargin: 20 * appScaleSize
                        right : parent.right
                        rightMargin: 20 * appScaleSize
                    }

                    height: languagesList.count * default_setting_item_height
                    clip: true
                    model: kcm.selectedTranslationsModel
                    delegate: languagesListItemComponent
                }
            }

            Text {
                id: language_info

                anchors {
                    left: parent.left
                    right:parent.right
                    top: list_area.bottom
                    leftMargin:  marginLeftAndRight * 2
                    rightMargin:  marginLeftAndRight * 2
                    topMargin: 9 * appScaleSize
                }

                text: i18n("The language at the top of the list will be used by default. If an app doesn't support your default language, the next language in the list will be used instead.")
                font.pixelSize:12 * appFontSize
                color: Kirigami.JTheme.disableForeground
                wrapMode : Text.WordWrap
            }

            Rectangle {
                id:regionRect

                anchors {
                    top : language_info.bottom
                    topMargin: 25 * appScaleSize
                    left: parent.left
                    leftMargin: marginLeftAndRight
                    right:parent.right
                    rightMargin: marginLeftAndRight
                }

                width:parent.width- marginLeftAndRight * 2
                height: 45 * appScaleSize
                color:Kirigami.JTheme.cardBackground
                radius: radiusCommon

                Text {
                    anchors {
                        left: regionRect.left
                        leftMargin: marginLeftAndRight
                        verticalCenter:parent.verticalCenter
                    }
                    height: 12 * appScaleSize
                    text: i18n("Region")
                    verticalAlignment:Text.AlignVCenter
                    font.pixelSize: 14 * appFontSize
                    color: Kirigami.JTheme.majorForeground
                }

                Text {
                    id:currentFormatText

                    anchors {
                        right: rightIcon.left
                        rightMargin: 5 * appScaleSize
                        verticalCenter: parent.verticalCenter
                    }
                    height: 12 * appScaleSize
                    text: regionManager.lcLocation
                    verticalAlignment:Text.AlignVCenter
                    font.pixelSize: 14 * appFontSize
                    color: Kirigami.JTheme.minorForeground
                }

                Kirigami.JIconButton {
                    id: rightIcon

                    anchors{
                        right: parent.right
                        rightMargin: marginLeftAndRight
                        verticalCenter: currentFormatText.verticalCenter
                    }

                    width: 30 * appScaleSize
                    height: width
                    source: Qt.resolvedUrl("../image/icon_right.png")
                    color: Kirigami.JTheme.iconMinorForeground
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        openSelectView()
                    }
                }
            }

            Rectangle {
                id: region_title

                anchors {
                    top : regionRect.bottom
                    topMargin: 24 * appScaleSize
                    left: regionRect.left
                    right: parent.right
                }
                width: parent.width
                height: default_setting_title_height
                color: "transparent"

                Text {
                    anchors {
                        left: region_title.left
                        leftMargin: marginLeftAndRight
                        verticalCenter:parent.verticalCenter
                    }
                    height: 12 * appScaleSize
                    text: i18n("Region Format Example")
                    verticalAlignment:Text.AlignVCenter
                    font.pixelSize: 12 * appFontSize
                    color: Kirigami.JTheme.disableForeground
                }
            }

            Column {
                id:regionDataRow

                anchors{
                    left: region_title.left
                    top: region_title.bottom
                    leftMargin: marginLeftAndRight
                    topMargin: 12 * appScaleSize
                }
                spacing: 5 * appScaleSize
                height: 200 * appScaleSize
                width: parent.width

                Repeater{
                    id:regionExample

                    model: regionArray.length
                    delegate: Rectangle {
                        id: item
                        width: regionDataRow.width
                        height: dataText.contentHeight
                        color: "transparent"
                        Text {
                            id: titleItem
                            text: regionArray[index] + ": "
                            width: 130 * appScaleSize
                            verticalAlignment:Text.AlignVCenter
                            horizontalAlignment: Text.AlignRight
                            font.pixelSize: 14 * appFontSize
                            color: Kirigami.JTheme.majorForeground
                        }
                        Text {
                            id: dataText

                            anchors{
                                left: titleItem.right
                                right: parent.right
                                rightMargin: 50 * appScaleSize
                            }
                            verticalAlignment:Text.AlignBottom
                            horizontalAlignment: Text.AlignLeft

                            wrapMode:Text.WrapAnywhere
                            text: regionValueArray[index]
                            font.pixelSize: 14 * appFontSize
                            color: Kirigami.JTheme.majorForeground
                        }
                    }
                }
            }
        }
    }

    Kirigami.JDialog {
        id: confirmDlg

        title: i18n("Restart")
        text: i18n("Applying this setting will restart your PAD")
        leftButtonText: i18n("Cancel")
        rightButtonText: i18n("Restart")
        dim: true
        focus: true

        onRightButtonClicked: {
            kcm.save()
            if(regionSelectIndex !== -2){
                regionManager.writeConfig(regionSelectIndex)
            }
            if (updateLanguage != "") {
                regionManager.writeLanguage(updateLanguage)
                updateLanguage = ""
            }
            translateTool.restartDevice()
            confirmDlg.close()
        }

        onLeftButtonClicked: {
            confirmDlg.close()
        }
    }


    QtControls.StackView{
       id: pageManger

        anchors.fill: parent
        Component.onCompleted: {
            push(mainComponent)
        }
    }

    function openSelectView(){
        regionManager.updateRegionModel("")
        var detailObject = pageManger.push(regionSelectComponent)
    }

    function exitSelectView(selectItemIndex){
        regionSelectIndex = selectItemIndex
        pageManger.pop()
    }

    Component{
        id: regionSelectComponent

        RegionSelectView {
            id: rsv
            width: root.width
            height: root.height
        }
    }
}
