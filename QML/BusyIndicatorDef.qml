import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: item1
    Text {
        id: text1
        color: "#2e72ea"
        text: qsTr("waiting for Design-Safe")
        anchors.fill: parent
        rotation: 0
        font.bold: true
        font.family: "Tahoma"
        clip: false
        renderType: Text.QtRendering
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        font.pixelSize: 28
    }

    Text {
        id: text2
        x: -3
        y: 4
        color: "#2eea2e"
        text: qsTr("waiting for Design-Safe")
        clip: false
        font.bold: true
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        renderType: Text.QtRendering
        wrapMode: Text.WordWrap
        rotation: 0
        font.pixelSize: 28
        anchors.fill: parent
        font.family: "Tahoma"
    }

    Text {
        id: text3
        x: -6
        y: 0
        color: "#eae02e"
        text: qsTr("waiting for Design-Safe")
        clip: false
        verticalAlignment: Text.AlignVCenter
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        renderType: Text.QtRendering
        wrapMode: Text.WordWrap
        rotation: 0
        font.pixelSize: 28
        anchors.fill: parent
        font.family: "Tahoma"
    }

    Text {
        id: text4
        x: 3
        y: 12
        color: "#eb4d2e"
        text: qsTr("waiting for Design-Safe")
        clip: false
        verticalAlignment: Text.AlignVCenter
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        renderType: Text.QtRendering
        wrapMode: Text.WordWrap
        rotation: 0
        font.pixelSize: 28
        anchors.fill: parent
        font.family: "Tahoma"
    }
}
