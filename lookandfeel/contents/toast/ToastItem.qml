/*
 * Copyright 2014 Martin Klapetek <mklapetek@kde.org>
 * Copyright 2019 Kai Uwe Broulik <kde@broulik.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.14
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.5
import org.kde.plasma.core 2.0 as PlasmaCore
//import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.extras 2.0 as PlasmaExtra
import QtQuick.Window 2.2

Rectangle{
    id:name

    property QtObject rootItem
    implicitWidth: iconArea.width * 1.1
    implicitHeight: content.implicitHeight
    color: "#A0000000"
    radius: name.height*0.3125
    anchors.top: parent.top
    anchors.horizontalCenter: parent.horizontalCenter
    RowLayout{
        spacing: 30
        anchors.centerIn: parent
        implicitWidth: content.implicitWidth
        Item {
            id:iconArea
            implicitWidth: content.contentWidth
            implicitHeight: content.implicitHeight
            Text {
                id: content
                anchors.fill:iconArea
                text:rootItem.toastContent
                color: "#ffffff"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                Component.onCompleted: {
                    name.width = width * 1.1
                }
                onTextChanged: {
                    name.width = width * 1.1
                }
            }
        }
    }
}

