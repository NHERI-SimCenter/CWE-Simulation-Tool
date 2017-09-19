import QtQuick 2.0
//import QtGraphicalEffects 1.0

Rectangle {
    id: main
    color: "#00000000"
    width: 300; height: 60

    Text {
        id: theMessage
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
        font.pixelSize: 20
    }

    SequentialAnimation {
        running: true
        loops: Animation.Infinite
        PropertyAnimation { target: theMessage; property: "color"; to: "#2e72ea"; duration: 1000 }
        PropertyAnimation { target: theMessage; property: "color"; to: "#2eea2e"; duration: 1000 }
        PropertyAnimation { target: theMessage; property: "color"; to: "#eae02e"; duration: 1000 }
        PropertyAnimation { target: theMessage; property: "color"; to: "#eb4d2e"; duration: 1000 }
    }

}
