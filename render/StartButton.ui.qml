import QtQuick 2.0

Item {
    id: button
    width: 92
    height: 27
    property alias mouseArea: mouseArea
    property string text: "No text"
    rotation: -7
    property alias text1: text1

    Rectangle {
        id: rectangle1
        x: 0
        y: 9
        width: 90
        height: 25
        radius: 13
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        border.color: "#b00000"
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#ffff00"
            }

            GradientStop {
                position: 0.772
                color: "#c00000"
            }

        }
        rotation: 0
        border.width: 1

        Text {
            id: text1
            x: 82
            y: 30
            width: 81
            height: 23
            text: button.text
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 12
        }
    }

    MouseArea {
        id: mouseArea
        x: 3
        y: -4
        anchors.fill: parent
    }


}
