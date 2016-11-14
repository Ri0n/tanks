import QtQuick 2.6
import QtQuick.Window 2.2


Window {
    visible: true
    width: 800
    height: 800
    title: qsTr("Hello World")


	MainForm {
		anchors.fill: parent
//		mouseArea.onClicked: {
//			Qt.quit();
//		}
	}

}
