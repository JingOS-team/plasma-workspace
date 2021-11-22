/*
 * Copyright 2014 Martin Klapetek <mklapetek@kde.org>
 * Copyright 2019 Kai Uwe Broulik <kde@broulik.de>
 * Copyright 2021 Liu Bangguo <liubangguo@jingos.com>
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
import jingos.display 1.0

Rectangle{
    id:name

    property QtObject rootItem

    implicitWidth: name.width
    implicitHeight: name.height
    color: "#80000000"
    radius: name.height*0.3125
    anchors.top: parent.top
    anchors.horizontalCenter: parent.horizontalCenter
    RowLayout{
        spacing: JDisplay.dp(8)
        anchors.centerIn: parent
        implicitWidth: iconArea.implicitWidth+progressBar.implicitWidth+name.width*0.0078
        Item {
            id:iconArea
            implicitWidth: JDisplay.dp(22)
            implicitHeight: JDisplay.dp(22)
            Image {
                id: barIcon
                anchors.fill:iconArea
                source: icon? "file:///usr/share/icons/jing/SwiMachine/"+icon+".svg":icon
            }
            ShaderEffect {
                anchors.fill: barIcon
                property variant src: barIcon
                property color color: "white"
                fragmentShader: "
                                varying highp vec2 qt_TexCoord0;
                                uniform sampler2D src;
                                uniform highp vec4 color;
                                uniform lowp float qt_Opacity;
                                void main() {
                                    lowp vec4 tex = texture2D(src, qt_TexCoord0);
                                    gl_FragColor = vec4(color.r * tex.a, color.g * tex.a, color.b * tex.a, tex.a) * qt_Opacity;
                                }"
            }
        }
        ProgressBar {
            id: progressBar

            implicitWidth:name.width*0.664
            implicitHeight: name.height*0.094

            hoverEnabled: true
            Layout.fillWidth: true

            from: 0
            to: rootItem.osdMaxValue
            value: Number(rootItem.osdValue)

            contentItem: Item {
                id:conten
                implicitWidth:name.width*0.664
                implicitHeight: name.height*0.094
                Rectangle{
                    id: indicator
                    height: parent.height
                    width: progressBar.visualPosition * parent.width
                    radius: conten.height/3
                    color: "#FFFFFFFF"
                }
            }

            background:Rectangle {
                implicitWidth: name.width*0.664
                implicitHeight: name.height*0.094
                color: "#4DEBEBF5"
                radius: conten.height/3
            }
        }
    }
    MouseArea{
        anchors.fill: parent
        onPressed: {

        }
        onMouseXChanged: {

        }
        onReleased: {


        }
    }
}

