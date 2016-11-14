import QtQuick 2.6
import com.rsoft.tanks 1.0

Rectangle {
	id: battleField

    property alias mouseArea: lowerLayer

    width: 200
    height: 200

	color: "red"



	Tanks {
		id: game
		property var tanksList: []
		property var bulletsList: []
		property var tanksMap: {}
		property var bulletsMap: {}
		property var pendingBlockRemove: []

		onMapRendered: {
			console.log("C++ map rendered");
			// destroy previous objects
			for (var i=0; i < game.tanksList.length; i++) {
				game.tanksList[i].destroy();
			}
			game.tanksList = [];
			game.tanksMap = {};

			for (var i=0; i < game.bulletsList.length; i++) {
				game.bulletsList[i].destroy();
			}
			game.bulletsList = [];
			game.bulletsMap = {};


			lowerLayer.lowerImage = "file://" + game.mapImageFilename
			lowerLayer.loadImage(lowerLayer.lowerImage)
			bushLayer.source = "file://" + game.bushImageFilename
		}

		onNewTank: {

			var source = 'import QtQuick 2.0; Image {\n' +
									'id:' + tank.id + '\n' +
									'property int animFrame: 0\n' +
									'property int tankLevel: ' + tank.variant + '\n' +
									'source: "image://tankprovider/' + tank.affinity + '/' + tank.variant + '/' + tank.direction + '/0"\n' +
									'width: ' + tank.geometry.width + '\n' +
									'height: ' + tank.geometry.height + '\n' +
									'smooth: false\n' +
									'x: ' + tank.geometry.x + '\n' +
									'y: ' + tank.geometry.y + '\n' +
									'z:50 }';
			console.log("New tank! " + source)
			var obj = Qt.createQmlObject(source, battleField, "dynamicTank_" + tank.id);
			game.tanksMap[tank.id] = obj;
			game.tanksList.push(obj)

		}

		onNewBullet: {
			var source = 'import QtQuick 2.0; Image {\n' +
									'id:' + bullet.id + '\n' +
									'source: "image://bulletprovider/' + bullet.direction + '"\n' +
									'width: ' + bullet.geometry.width + '\n' +
									'height: ' + bullet.geometry.height + '\n' +
									'smooth: false\n' +
									'x: ' + bullet.geometry.x + '\n' +
									'y: ' + bullet.geometry.y + '\n' +
									'z:50 }';
			console.log("New bullet! " + source)
			var obj = Qt.createQmlObject(source, battleField, "dynamicBullet_" + bullet.id);
			game.bulletsMap[bullet.id] = obj;
			game.bulletsList.push(obj)
		}

		onBulletMoved: {
			game.bulletsMap[id].x = pos.x;
			game.bulletsMap[id].y = pos.y;
		}

		onBulletRemoved: {
			console.log("Destroy bullet:" + id);
			var obj = game.bulletsMap[id]
			var index = game.bulletsList.indexOf(obj);
			game.bulletsList.splice(index,1)
			delete game.bulletsMap[id]
			obj.destroy();
		}

		onTankUpdated: {
			game.tanksMap[tank.id].x = tank.geometry.x
			game.tanksMap[tank.id].y = tank.geometry.y
			game.tanksMap[tank.id].source = 'image://tankprovider/' + tank.affinity + '/' + tank.variant + '/' + tank.direction + '/0';
		}

		onBlockRemoved: {
			game.pendingBlockRemove.push(block);
			lowerLayer.markDirty(block)
			console.log("Dirty: " + block);
		}
	}

	Canvas {
		id: lowerLayer
		anchors.fill: parent

		canvasSize : game.boardImageSize
		canvasWindow  : Qt.rect(0, 0, game.boardImageSize.width, game.boardImageSize.height)
		tileSize : Qt.size(100, 100)

		property string lowerImage: ""
		property bool lowerRendered: false

		onImageLoaded : {
			if (lowerLayer.lowerImage) {
				console.log("Image loaded: " + lowerLayer.lowerImage);
				console.log(game.boardImageSize)
				battleField.width = game.boardImageSize.width;
				battleField.height = game.boardImageSize.height;
				requestPaint();
			}
		}

		onPaint: {
			console.log("Render");
			if (!lowerLayer.lowerRendered && lowerLayer.lowerImage &&
			     lowerLayer.isImageLoaded(lowerLayer.lowerImage))
		    {
				var ctx = getContext("2d")
				ctx.fillStyle = "black"
				ctx.fillRect(0,0,lowerLayer.canvasSize.width, lowerLayer.canvasSize.height )
				ctx.drawImage(lowerLayer.lowerImage, 0,0)
				lowerLayer.lowerRendered = true
			}
			if (game.pendingBlockRemove.length) {
				console.log("Has pending remove");
				var ctx = getContext("2d")
				ctx.fillStyle = "black"

				for (var i = 0; i < game.pendingBlockRemove.length; i++) {
					console.log("Remove: " + game.pendingBlockRemove[i]);
					var b = game.pendingBlockRemove[i];
					ctx.fillRect(b.x, b.y, b.width, b.height)
				}
				game.pendingBlockRemove = []
			} else {
			    console.log("No pending remove");
			}
		}
	}

	Image {
		id: bushLayer
		z: 100
	}


	focus: true
	Keys.onPressed: {
		if(event.isAutoRepeat) return

		/* see basics.h
		North = 0
	    South = 1
	    West  = 2
	    East  = 3
		----
		Fire  = 4
		*/
		var isMove = false;
		var dir = -1
		var p1keys = [Qt.Key_Up, Qt.Key_Down, Qt.Key_Left, Qt.Key_Right, Qt.Key_Space]
		var p2keys = [Qt.Key_W, Qt.Key_S, Qt.Key_A, Qt.Key_D, Qt.Key_Enter]

		var p1res = p1keys.indexOf(event.key);
		if (p1res != -1) {
			console.log("Player 1 action = " + p1res);
			event.accepted = true;
			game.qmlTankAction(0, p1res);

		}

		var p2res = p2keys.indexOf(event.key);
		if (p2res != -1) {
			console.log("Player 2 action = " + p2res);
			event.accepted = true;
			game.qmlTankAction(1, p2res);

		}
//		if (event.key == Qt.Key_Left) {
//			console.log("move left");
//			dir = 2;
//			event.accepted = true;
//			myTank.x -= 4
//			isMove = true
//		}
//		if (event.key == Qt.Key_Right) {
//			console.log("move right");
//			event.accepted = true;
//			myTank.x += 4
//			isMove = true
//		}
//		if (event.key == Qt.Key_Up) {
//			console.log("move up");
//			event.accepted = true;
//			myTank.y -= 4
//			isMove = true
//		}
//		if (event.key == Qt.Key_Down) {
//			console.log("move down");
//			event.accepted = true;
//			myTank.y += 4
//			isMove = true
//		}

//		if (isMove) {
//			myTank.animFrame++
//			myTank.animFrame%=2
//			console.log("Anim frame:" + myTank.animFrame)
//			myTank.source = "image://tankprovider/mytank/" + myTank.tankLevel +
//			                "/" + myTank.animFrame
//		}
	}

	Keys.onReleased: {
		if(event.isAutoRepeat) return

		var p1keys = [Qt.Key_Up, Qt.Key_Down, Qt.Key_Left, Qt.Key_Right, Qt.Key_Space]
		var p2keys = [Qt.Key_W, Qt.Key_S, Qt.Key_A, Qt.Key_D, Qt.Key_Enter]

		var p1res = p1keys.indexOf(event.key);
		if (p1res != -1) {
			console.log("Player 1 action = " + p1res);
			event.accepted = true;
			game.qmlTankActionStop(0, p1res);

		}

		var p2res = p2keys.indexOf(event.key);
		if (p2res != -1) {
			console.log("Player 2 action = " + p2res);
			event.accepted = true;
			game.qmlTankActionStop(1, p2res);

		}
	}

}
