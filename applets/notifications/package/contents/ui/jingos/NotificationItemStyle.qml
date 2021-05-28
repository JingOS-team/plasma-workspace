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
 
import QtQuick 2.0

QtObject {
    property int width: 260
    property int minHeight: 97
    property int padding: 10
    property int space: 0
    property int iconSize: 15
    property int actionIconSize: 22

    property color backgroundColor: "#FFFFFFFF"
    property int backgroundRadius: 9
    property bool backgroundShadowEnabled: true
    property color backgroundShadowColor: "#F0A0A0A0"
    property int backgroundShadowRadius: 16
    property int backgroundShadowSamples: 33
    property bool backgroundShadowBorder: true
    property int backgroundShadowHOffset: 0
    property int backgroundShadowVOffset: 0

    property int headerLeftMargin: 9

    property color headerColor: "#99000000"
    property color timeColor: "#99000000"
    property color summaryColor: "#FF000000"
    property color contentColor: "#FF000000"
    property font headerFont: Qt.font({
                                          "bold": false,
                                          "pixelSize": 13
                                      })
    property font summaryFont: Qt.font({
                                          "bold": true,
                                          "pixelSize": 14
                                      })
    property font contentFont: Qt.font({
                                          "bold": false,
                                          "pixelSize": 14
                                      })
    property font timeFont: Qt.font({
                                          "bold": false,
                                          "pixelSize": 11
                                      })

    property int animationTime: 200
}
