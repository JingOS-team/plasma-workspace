/*
 *  Copyright 2013 Marco Martin <mart@kde.org>
 *  Copyright 2014 Sebastian Kügler <sebas@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  2.010-1301, USA.
 */

import QtQuick 2.0
import org.kde.kquickcontrolsaddons 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import jingos.display 1.0

MouseArea {
    id: wallpaperDelegate

    width: wallpapersGrid.delegateWidth
    height: wallpapersGrid.delegateHeight

    property bool selected: (wallpapersGrid.currentIndex == index)

    hoverEnabled: true

    PlasmaCore.FrameSvgItem {
        id: frameSvg
        imagePath: "widgets/media-delegate"
        prefix: (wallpapersGrid.currentIndex - (wallpapersGrid.currentPage*wallpapersGrid.pageSize)) == index ? "picture-selected" : "picture"
        width: (wallpapersGrid.currentIndex - (wallpapersGrid.currentPage*wallpapersGrid.pageSize)) == index ? parent.width+JDisplay.dp(5) : parent.width-JDisplay.dp(16)
        height: (wallpapersGrid.currentIndex - (wallpapersGrid.currentPage*wallpapersGrid.pageSize)) == index ? parent.height+JDisplay.dp(5) : parent.height-JDisplay.dp(16)
        anchors {
            left: parent.left
            right: parent.right
            bottomMargin: JDisplay.dp(8)
        }

        Behavior on width {
            NumberAnimation {
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }
        Behavior on height {
            NumberAnimation {
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }
        QPixmapItem {
            id: walliePreview
            anchors {
                fill: parent
                leftMargin: parent.margins.left
                topMargin: parent.margins.top
                rightMargin: parent.margins.right
                bottomMargin: parent.margins.bottom
            }
            visible: model.screenshot !== null
            smooth: true
            pixmap: model.screenshot
            fillMode: QPixmapItem.Stretch
        }
    }

    onClicked: {
        wallpapersGrid.currentIndex = (wallpapersGrid.currentPage*wallpapersGrid.pageSize) + index
        cfg_Image = model.path
    }
}
