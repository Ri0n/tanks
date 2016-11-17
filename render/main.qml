import QtQuick 2.6
import QtQuick.Window 2.2


Window {
    visible: true
    width: 900
    height: 800
    title: qsTr("Battle City: The War")

//	Rectangle {
//		width: 100
//		height: 100
//		anchors.fill: parent
//		Text {
//			anchors.fill: parent
//			text: "<b>Restart!</b>"
//			font.pointSize: 12
//			color: "white"
//			horizontalAlignment: Text.AlignHCenter
//			verticalAlignment: Text.AlignVCenter
//		}

//	}

	MainForm {
		anchors.fill: parent
	}

}
