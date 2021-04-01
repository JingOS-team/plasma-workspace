import QtQuick 2.0

QtObject {
    property int width: 520
    property int minHeight: 160
    property int padding: 19
    property int space: 10
    property int iconSize: 30
    property int actionIconSize: 44

    property color backgroundColor: "#FFFFFFFF"
    property int backgroundRadius: 18
    property bool backgroundShadowEnabled: true
    property color backgroundShadowColor: "#F0A0A0A0"
    property int backgroundShadowRadius: 16
    property int backgroundShadowSamples: 33
    property bool backgroundShadowBorder: true
    property int backgroundShadowHOffset: 0
    property int backgroundShadowVOffset: 0

    property int headerLeftMargin: 16

    property color headerColor: "#99000000"
    property color timeColor: "#99000000"
    property color summaryColor: "#FF000000"
    property color contentColor: "#FF000000"
    property font headerFont: Qt.font({
                                          "bold": false,
                                          "pixelSize": 26
                                      })
    property font summaryFont: Qt.font({
                                          "bold": true,
                                          "pixelSize": 28
                                      })
    property font contentFont: Qt.font({
                                          "bold": false,
                                          "pixelSize": 28
                                      })
    property font timeFont: Qt.font({
                                          "bold": false,
                                          "pixelSize": 22
                                      })

    property int animationTime: 200
}
