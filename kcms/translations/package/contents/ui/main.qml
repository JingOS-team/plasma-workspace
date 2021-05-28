/*
 * Copyright (C) 2015 Marco Martin <mart@kde.org>
 * Copyright (C) 2018 Eike Hein <hein@kde.org>
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

Rectangle {
    id: root


    property int screenWidth: 888
    property int screenHeight: 648

    property int statusbar_height : 22
    property int statusbar_icon_size: 22
    property int default_setting_item_height: 45
    property int default_setting_title_height: 30

    property int marginTitle2Top : 44 
    property int marginItem2Title : 36
    property int marginLeftAndRight : 20 
    property int marginItem2Top : 24
    property int radiusCommon: 10 
    property int fontNormal: 14 

    width: screenWidth * 0.7
    height: screenHeight

    TranslateTool {
        id: translateTool
    }

    Component {
        id: languagesListItemComponent

        Item {
            width: languagesList.width - 5
            height: listItem.implicitHeight

            Kirigami.SwipeListItem {
                id: listItem
                height: 45
                spacing: 0

                background:Rectangle{
                    color:"transparent"

                    Kirigami.Separator {
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: marginLeftAndRight
                        anchors.rightMargin: marginLeftAndRight
                        anchors.bottomMargin: -9
                        color: "#f0f0f0"
                        visible: index != languagesList.count -1 
                    }
                }

                contentItem: RowLayout {
                    Kirigami.ListItemDragHandle {
                        listItem: listItem
                        listView: languagesList
                        onMoveRequested: {
                            kcm.selectedTranslationsModel.move(oldIndex, newIndex)
                            confirmDlg.open()
                        }
                    }

                    Kirigami.Icon {
                        // visible: model.IsMissing
                        visible: false 

                        Layout.alignment: Qt.AlignVCenter

                        width: 17
                        height: width

                        source: "../image/icon_move.png"
                        color: Kirigami.Theme.negativeTextColor
                    }

                    QtControls.Label {
                        Layout.fillWidth: true

                        Layout.alignment: Qt.AlignVCenter

                        text:  model.display
                        color: (index == 0) ? "#FF3C4BE8" :"#000"

                        font.pixelSize: 16
                    }

                    Image {
                        source:"../image/menu_select.png"
                        width: 22
                        height: 22
                        sourceSize.width: 22
                        sourceSize.height: 22
                        anchors {
                            right:parent.right
                            rightMargin: marginLeftAndRight
                            verticalCenter:parent.verticalCenter
                        }
                        visible: index == 0
                    }
                }
            }
        }
    }

    Rectangle{
        width: parent.width
        height: parent.height 
        color: "#FFF6F9FF"

        Text {
            id: title

            anchors {
                left: parent.left
                top: parent.top
                leftMargin: marginLeftAndRight  
                topMargin: marginTitle2Top  
            }

            width: 329
            height: 14
            text: i18n("Language")
            font.pixelSize: 20 
            font.weight: Font.Bold
        }

        Rectangle {
            id:list_area

            anchors {
                top : title.bottom
                topMargin: marginItem2Title
                left: parent.left
                leftMargin: marginLeftAndRight
                right:parent.right
                rightMargin: marginLeftAndRight
            }

            width:parent.width- marginLeftAndRight * 2
            height:languagesList.height + 10 + language_title.height 
            color:"white"
            radius: radiusCommon

            Rectangle {
                id: language_title
                anchors {
                    top : parent.top
                    left: parent.left
                    right: parent.right 
                }
                width: parent.width
                height: default_setting_title_height
                radius: radiusCommon

                 Text {
                    anchors {   
                        left: language_title.left
                        leftMargin: marginLeftAndRight 
                        verticalCenter:parent.verticalCenter
                    }
                    height: 12
                    text: i18n("Preferred language order")
                    verticalAlignment:Text.AlignVCenter
                    font.pixelSize: 12
                    color: "#4D000000"
                }
            }


            ListView {
                id: languagesList

                anchors {
                    top : language_title.bottom
                    left: parent.left
                    leftMargin: 7 
                    right : parent.right
                    rightMargin: 7 
                }

                width: root.width - marginLeftAndRight * 2
                height: languagesList.count * default_setting_item_height + 15 

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
                topMargin: 9
            }
            
            text: i18n("The language at the top of the list will be used by default. If an app doesn't support your default language, the next language in the list will be used instead.")
            // font.pointSize: appFontSize - 2
            font.pixelSize:12
            color: "#4D000000"
            wrapMode : Text.WordWrap
        }
    }

    Kirigami.JDialog {
        id: confirmDlg

        title: i18n("Restart")
        text: i18n("Applying this setting will restart your PAD")
        leftButtonText: i18n("Cancel")
        rightButtonText: i18n("Restart")
        // rightButtonTextColor: "red"
        dim: true
        focus: true
 
        onRightButtonClicked: {
            kcm.save()
            translateTool.restartDevice()
            confirmDlg.close()
        }

        onLeftButtonClicked: {
            kcm.load() 
            confirmDlg.close()
        }
    }
    
}
