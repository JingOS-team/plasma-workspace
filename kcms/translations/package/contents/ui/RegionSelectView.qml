/*
 * Copyright (C) 2021 Beijing Jingling Information System Technology Co., Ltd. All rights reserved.
 *
 * Authors:
 * Liu Bangguo <liubangguo@jingos.com>
 *
 */
import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.3
import org.kde.kirigami 2.15 as Kirigami
Rectangle {
    id:detailRect

    property var currentSelectHeadStart
    color: Kirigami.JTheme.settingMinorBackground//"#FFF6F9FF"
    property variant listModels: [
         "#","A","B","C","D","E","F"
         ,"G","H","I","J","K","L"
         ,"M","N","O","P","Q"
         ,"R","S","T","U","V"
         ,"W","X","Y","Z"]

    signal confimDialogOpenChanged()

    Kirigami.JDialog {
        id: updateRegionDialog

        title: i18n("Restart")
        text: i18n("Applying this setting will restart your PAD")
        leftButtonText: i18n("Cancel")
        rightButtonText: i18n("Restart")
        // rightButtonTextColor: "red"
        dim: true
        focus: true

        onRightButtonClicked: {
            // translateTool.confirmDlg(true)
            console.log("onRightButtonClicked")
            regionManager.writeConfig(regionSelectListView.selectIndex)
            translateTool.restartDevice()
            updateRegionDialog.close()
        }

        onLeftButtonClicked: {
            // translateTool.confirmDlg(false)
            console.log("onLeftButtonClicked")
            updateRegionDialog.close()
            regionSelectListView.selectIndex = regionManager.regionSystemIndex
        }
    }

    Item {
        id: regionSelectTitle

        anchors {
            top: parent.top
            topMargin: 42 * appScaleSize
            left: parent.left
            leftMargin: 20 * appScaleSize
        }
        height: backLabel.height
        width: parent.width

        Kirigami.JIconButton {
            id: backLabel

            anchors {
                left: parent.left
            }

            width: 30 * appScaleSize
            height: width

            source: "qrc:/image/arrow_left.png"
            color: Kirigami.JTheme.iconForeground

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    if(regionSelectListView.selectIndex !== regionManager.regionSystemIndex & regionSelectListView.selectIndex !== -1){
                        regionManager.showUpdateExampleByIndex(regionSelectListView.selectIndex)
                        isUpddateLang = true
                    }
//                    }else {
                    exitSelectView(regionSelectListView.selectIndex)
//                    }
                }
            }
        }

        Kirigami.Label {
            id:systemTitle
            anchors {
                left: backLabel.right
                leftMargin: 11 * appScaleSize
                verticalCenter: backLabel.verticalCenter
            }

            font.pixelSize: 20 * appFontSize
            font.bold: true
            text: i18n("Select Region")
            color: Kirigami.JTheme.majorForeground
        }
    }

    Kirigami.JSearchField {
            id: searchRect

            anchors{
                top: regionSelectTitle.bottom
                topMargin: 20 * appScaleSize
                left: regionSelectTitle.left
                right: parent.right
                rightMargin: 20 * appScaleSize
            }
            width: parent.width - 40 * appScaleSize

            focus: false
            font.pixelSize: 17 * appFontSize
            placeholderText: ""
            Accessible.name: i18n("Search")
            Accessible.searchEdit: true

            // background: Rectangle {
            //     anchors.fill: parent
            //     color: "#FFFFFF"
            //     radius: height * 0.36
            // }

            onRightActionTrigger: {
            }

            onTextChanged: {
                console.log(" ****onTextChanged text:"+ text)
                regionManager.updateRegionModel(text)
                if(text !== ""){
                    regionSelectListView.selectIndex = -1
                    regionSelectListView.section.delegate = null
                } else {
                    regionSelectListView.selectIndex = regionManager.regionSystemIndex
                    regionSelectListView.section.delegate = sectionHeading
                }
            }

            onAccepted: {
            }
        }

    Rectangle {
        id:regionListRect

        anchors {
            top : searchRect.bottom
            topMargin: 27 * appScaleSize
            left: searchRect.left
            right:searchRect.right
            bottom: parent.bottom
            bottomMargin: 40 * appScaleSize
        }

        width:parent.width
        color:Kirigami.JTheme.cardBackground//"white"
        radius: radiusCommon
        clip: true

        Component {
            id: regionItemComponent

            Item {
                property bool currentIndexSelected: index === regionSelectListView.selectIndex
                width: regionSelectListView.width
                height: listItem.height

                Kirigami.SwipeListItem {
                    id: listItem
                    height: 45 * appScaleSize
                    spacing: 0
                    leftPadding: 0
                    rightPadding: 0
                    
                    onClicked: {
                        console.log(" region item selected:" + index)
                        regionSelectListView.selectIndex = index
                        if(regionSelectListView.selectIndex !== regionManager.regionSystemIndex & regionSelectListView.selectIndex !== -1){
                            regionManager.showUpdateExampleByIndex(regionSelectListView.selectIndex)
                            isUpddateLang = true
                        }
                        exitSelectView(regionSelectListView.selectIndex)
                    }

                    background:Rectangle{
                        color:"transparent"
                        Kirigami.Separator {
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: parent.right
                            color: Kirigami.JTheme.dividerForeground//"#f0f0f0"
                            visible: index != regionSelectListView.count -1
                        }
                    }

                    contentItem: Rectangle {
                        color:"transparent"
                        Label {

                            anchors {
                               left: parent.left
                               verticalCenter:parent.verticalCenter
                            }
                            text:  model.RegionName
                            color: currentIndexSelected ? Kirigami.JTheme.highlightColor : Kirigami.JTheme.majorForeground//"#FF3C4BE8" :"#000"

                            font.pixelSize: 16 * appFontSize
                            onColorChanged: {
                                if(color === Kirigami.JTheme.highlightColor) {
                                    currentSelectHeadStart = model.RegionName[0]
                                    console.log(" current select head start:::::" + currentSelectHeadStart)
                                }
                            }
                        }
                        Component{
                            id:selectComponent
                            Image {
                                source:"../image/menu_select.png"
                                width: 22 * appScaleSize
                                height: 22 * appScaleSize
                                // sourceSize.width: 22
                                // sourceSize.height: 22
                            }
                        }
                        Loader{
                           id:selectedLoader
                           sourceComponent: selectComponent
                           active:currentIndexSelected
                           anchors {
                               right:parent.right
                               rightMargin: marginLeftAndRight
                               verticalCenter:parent.verticalCenter
                           }
                        }

                    }
                }
            }
        }

        Component {
            id: sectionHeading
            Rectangle {

                required property string section

                anchors {
                    left: parent.left
                    // leftMargin: 10 * appScaleSize
                }
                width: regionSelectListView.width
                height:isMatch(section) ? 0 : 40 * appScaleSize
                color: "transparent"
                visible: height !== 0

                function isMatch(section){
                    var han = /^[\u4e00-\u9fa5]+$/;
                    return han.test(section)
                }


                Text {
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 7 * appScaleSize
                    text: isMatch(section) ? "" : parent.section
                    font.pixelSize: 17 * appFontSize
                    color: currentSelectHeadStart === parent.section ? Kirigami.JTheme.highlightColor : Kirigami.JTheme.minorForeground//"#4D000000"
                }
                Kirigami.Separator {
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        color: Kirigami.JTheme.dividerForeground//"#f0f0f0"
                    }
            }
        }

        ListView {
            id: regionSelectListView
            property var selectIndex
            property string tipString

            anchors {
                left: parent.left
                leftMargin: 20 * appScaleSize
                right : parent.right
                rightMargin: 20 * appScaleSize
            }

            Component.onCompleted: {
                console.log(" system region index:" + regionManager.regionSystemIndex)
                // var showIndex = (count - regionManager.regionSystemIndex > 6) ?  (regionManager.regionSystemIndex + 4) : (count - 1)
                // regionSelectListView.currentIndex = showIndex
                regionManager.cacheSectionIndex()
                positionViewAtIndex(regionManager.regionSystemIndex,ListView.Beginning)
                regionSelectListView.selectIndex = regionManager.regionSystemIndex
                console.log(" system region regionSelectListView.selectIndex:" + regionSelectListView.selectIndex)
            }

            width: parent.width
            height: parent.height
            highlightMoveDuration: 0
            section.property: "RegionHead"
            section.criteria: ViewSection.FullString
            section.delegate: sectionHeading

            model: regionManager
            delegate: regionItemComponent

            Rectangle{ 
                id: tipText
                color: Kirigami.JTheme.dividerForeground
                width: 50 * appScaleSize
                height: 50 * appScaleSize
                anchors.centerIn: parent
                visible: regionSelectListView.tipString !== ""
                radius: 10 * appScaleSize
                Text{
                    id:itemText
                    anchors.centerIn: parent
                    color: Kirigami.JTheme.highlightColor
                    font.pixelSize: 16 * appFontSize
                    horizontalAlignment:  Text.AlignHCenter
                    text: regionSelectListView.tipString
                }
            }
        }


        Timer{
            id: tipTimer
            interval: 800
            onTriggered:{
                console.log(" tipTimer onTriggered::::::")
                regionSelectListView.tipString = ""
            }
        }

        ListView {
            id:alphabetList
            anchors{ 
                left: regionSelectListView.right
                leftMargin: 7 * appScaleSize
                top: parent.top
                topMargin: 24 * appScaleSize
            }
            model: listModels
            width: 20 * appScaleSize
            height: parent.height
            // spacing: 3 * appScaleSize
            boundsBehavior: Flickable.StopAtBounds

            delegate: Item {
                id:alphabetItem
                width: 10 * appScaleSize
                height: itemText.contentHeight + 3 * appScaleSize
                    Text{
                    id:itemText
                    anchors.centerIn: parent
                    color: Kirigami.JTheme.highlightColor
                    font.pixelSize: 10 * appFontSize
                    horizontalAlignment:  Text.AlignHCenter
                    text: listModels[index]
                    }
                    MouseArea{
                        width: parent.height + 5
                        height: parent.height + 5
                        onClicked:{
//                            console.log(" right clicked::::::" + sectionMapIndex[itemText.text] + " text:" + itemText.text);
//                            regionSelectListView.positionViewAtIndex(sectionMapIndex[itemText.text],ListView.Beginning)
                        }
                    }
                }

            function updateRegionIndex(mouse){
                var itemIndex = alphabetList.indexAt(mouse.x,mouse.y)
                console.log(" right onPositionChanged:::index:::" +itemIndex)
                if(itemIndex >= 0){
                    var regionIndex = regionManager.readSectionIndex(listModels[itemIndex])
                    regionSelectListView.tipString = listModels[itemIndex]
                    console.log(" right onPositionChanged:::regionIndex:::" +regionIndex)
                    if(regionIndex < 0){
                      while(itemIndex > 0 && regionIndex < 0){
                         itemIndex -- ;
                         regionIndex = regionManager.readSectionIndex(listModels[itemIndex])
                      }
                    }
                    regionSelectListView.positionViewAtIndex(regionIndex,ListView.Beginning)
                    if(tipTimer.running){
                        tipTimer.restart()
                    } else {
                        tipTimer.start()
                    }
                }
            }
        
            MouseArea{
                width: parent.height + 5
                height: parent.height + 5
                hoverEnabled: true
                onPositionChanged:{
                    console.log(" right onPositionChanged::::::" +mouse.x + " y:::" + mouse.y)
                    alphabetList.updateRegionIndex(mouse)
                }
                onClicked:{
                    console.log(" right onClicked::::::" +mouse.x + " y:::" + mouse.y)
                    alphabetList.updateRegionIndex(mouse)
                }
            }
        
        
        }
    }

}
