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

		/* see basics.h
		North = 0
		South = 1
		West  = 2
		East  = 3
		----
		Fire  = 4
		*/
		property var p1keys: [Qt.Key_W, Qt.Key_S, Qt.Key_A, Qt.Key_D, Qt.Key_Space]
		property var p2keys: [Qt.Key_Up, Qt.Key_Down, Qt.Key_Left, Qt.Key_Right, Qt.Key_Enter]

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

			var g = game.flagGeometry
			flag.x = g.x
			flag.y = g.y
			flag.width = g.width
			flag.height = g.height
			flag.source = game.flagFile

		}

		onFlagChanged: { flag.source = game.flagFile; }

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
									'Behavior on x { NumberAnimation { duration:150 } }\n' +
									'Behavior on y { NumberAnimation { duration:150 } }\n' +
									'z:50 }';
			//console.log("New tank! " + source)
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
									'Behavior on x { NumberAnimation { duration:150 } }\n' +
									'Behavior on y { NumberAnimation { duration:150 } }\n' +
									'z:50 }';
			//console.log("New bullet! " + source)
			var obj = Qt.createQmlObject(source, battleField, "dynamicBullet_" + bullet.id);
			game.bulletsMap[bullet.id] = obj;
			game.bulletsList.push(obj)

			var source = 'import QtMultimedia 5.6; Audio { source: "audio/fire"; }'; // audioRole: GameRole; does not work here
			var audio = Qt.createQmlObject(source, battleField, "dynamicBulletAudio_" + bullet.id);
			audio.stopped.connect(audio.destroy)
			audio.play()
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
			var old = game.tanksMap[tank.id]
			old.x = tank.geometry.x
			old.y = tank.geometry.y
			old.source = 'image://tankprovider/' + tank.affinity +
				'/' + tank.variant + '/' + tank.direction + '/' + old.animFrame;
			old.animFrame = (old.animFrame + 1) % 2;
		}

		onTankDestroyed: {
			var t = game.tanksMap[id]
			console.log("Tank destroy center:"+t.rect)
			var index = game.tanksList.indexOf(t);
			game.tanksList.splice(index,1)
			t.destroy()
			delete game.tanksMap[id]
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

	Image {
		id: flag
		smooth: false
	}


	function handleKeyEvent(event, gameHandler)
	{
		if(event.isAutoRepeat) return

		var p1res = game.p1keys.indexOf(event.key);
		if (p1res != -1) {
			event.accepted = true;
			gameHandler(0, p1res);

		}

		var p2res = game.p2keys.indexOf(event.key);
		if (p2res != -1) {
			event.accepted = true;
			gameHandler(1, p2res);

		}

	}

	focus: true
	Keys.onPressed: {
		handleKeyEvent(event, game.qmlTankAction)
	}

	Keys.onReleased: {
		handleKeyEvent(event, game.qmlTankActionStop)
	}

}
