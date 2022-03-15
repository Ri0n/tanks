/*
 Copyright (c) 2016, Sergey Ilinykh
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL IL'INYKH SERGEY BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

import QtQuick 2.6
import QtMultimedia
import com.rsoft.tanks 1.0

Row {
    anchors.fill: parent
    Rectangle {
        id: battleField

        //anchors.left: otherItem.left

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
            property var p2keys: [Qt.Key_Up, Qt.Key_Down, Qt.Key_Left, Qt.Key_Right, Qt.Key_Shift]

            property var tanksList: []
            property var bulletsList: []
            property var tanksMap: ({})
            property var bulletsMap: ({})
            property var pendingBlockRemove: []

            function bigExplosion(x, y, width, height, id) {
                var source = 'import QtQuick 2.0; AnimatedSprite { id:explode; z:200; source: "img/explosion1"; frameHeight: 16; frameWidth: 16; frameRate:6; frameCount:3; '+
                        'x:'+ x + "; y:" + y + '; width:' + width + '; height:' + height +
                        ';ParallelAnimation { running: true; ' +
                        'NumberAnimation { target:explode; property:"x"; to: '+(x-width)+'; duration: 500; }\n' +
                        'NumberAnimation { target:explode; property:"y"; to: '+(y-width)+'; duration: 500; }\n' +
                        'NumberAnimation { target:explode; property:"width"; to: '+(3*width)+'; duration: 500; }\n' +
                        'NumberAnimation { target:explode; property:"height"; to: '+(3*height)+'; duration: 500; }\n' +
                        '} }'
                var obj = Qt.createQmlObject(source, battleField, "bigExplosion_" + id);
                obj.destroy(500)
            }

            onMapRendered: {
                console.log("C++ map rendered");
                // destroy previous objects
                for (var i=0; i < game.tanksList.length; i++) {
                    game.tanksList[i].destroy();
                }
                game.tanksList = [];
                game.tanksMap = {};

                for (let i=0; i < game.bulletsList.length; i++) {
                    game.bulletsList[i].destroy();
                }
                game.bulletsList = [];
                game.bulletsMap = {};

                game.pendingBlockRemove = []
                lowerLayer.lowerRendered = false
                lowerLayer.lowerMapImage = "image://mapprovider/" + game.bridgeId + "/map"

                if (lowerLayer.isImageLoaded(lowerLayer.lowerMapImage) ||
                        lowerLayer.isImageLoading(lowerLayer.lowerMapImage))
                    lowerLayer.unloadImage(lowerLayer.lowerMapImage)

                lowerLayer.loadImage(lowerLayer.lowerMapImage)
                bushLayer.source = ""
                bushLayer.source = "image://mapprovider/" + game.bridgeId + "/bush"

                var g = game.flagGeometry
                flag.x = g.x
                flag.y = g.y
                flag.width = g.width
                flag.height = g.height
                flag.source = game.flagFile

            }

            onFlagChanged: {
                flag.source = game.flagFile;
                bigExplosion(flag.x, flag.y, flag.width, flag.height)
            }

            onNewTank: function(tank) {

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

            onNewBullet: function(bullet) {
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

                shotSound.play();
            }

            onBulletMoved: function(id, pos) {
                game.bulletsMap[id].x = pos.x;
                game.bulletsMap[id].y = pos.y;
            }

            onBulletDetonated: function(id, reason) {
                switch (reason) {
                case 0: // no damage
                    explNoDamageSound.play();
                    break;
                case 1: // bricks
                    explBrickSound.play();
                    break;
                case 2: // tank
                    explTankSound.play();
                    break;
                case 3: // flag
                    explFlagSound.play();
                    break;
                }
                var obj = game.bulletsMap[id]
                var index = game.bulletsList.indexOf(obj);
                game.bulletsList.splice(index,1)
                delete game.bulletsMap[id]
                obj.destroy();
            }

            onTankUpdated: function(tank) {
                //console.log("tank id=" + tank.id + " moved")
                var old = game.tanksMap[tank.id]
                if (old === undefined) {
                    console.log("Something went wrong. tank id=" + tank.id +  " is not found");
                    return;
                }

                old.x = tank.geometry.x
                old.y = tank.geometry.y
                old.source = 'image://tankprovider/' + tank.affinity +
                        '/' + tank.variant + '/' + tank.direction + '/' + old.animFrame;
                old.animFrame = (old.animFrame + 1) % 2;
                //console.log("x=" + old.x +  " y=" + old.y);
            }

            onTankDestroyed: function(id) {
                var t = game.tanksMap[id]

                game.bigExplosion(t.x, t.y, t.width, t.height, id)

                var index = game.tanksList.indexOf(t);
                game.tanksList.splice(index,1)
                t.destroy()
                delete game.tanksMap[id]
            }

            onBlockRemoved: function(block) {
                game.pendingBlockRemove.push(block);
                lowerLayer.markDirty(block)
                //console.log("Dirty: " + block);
            }
        }

        Canvas {
            id: lowerLayer
            anchors.fill: parent

            canvasSize : game.boardImageSize
            canvasWindow  : Qt.rect(0, 0, game.boardImageSize.width, game.boardImageSize.height)
            tileSize : Qt.size(100, 100)

            property string lowerMapImage: ""
            property bool lowerRendered: false

            onImageLoaded : {
                if (lowerLayer.lowerMapImage) {
                    battleField.width = game.boardImageSize.width;
                    battleField.height = game.boardImageSize.height;
                    requestPaint();
                }
            }

            onPaint: {
                //console.log("Render");
                if (!lowerLayer.lowerRendered && lowerLayer.lowerMapImage &&
                        lowerLayer.isImageLoaded(lowerLayer.lowerMapImage))
                {
                    var ctx = getContext("2d")
                    ctx.reset();
                    ctx.fillStyle = "black"
                    ctx.fillRect(0,0,lowerLayer.canvasSize.width, lowerLayer.canvasSize.height )
                    ctx.drawImage(lowerLayer.lowerMapImage, 0,0)
                    lowerLayer.lowerRendered = true
                }
                if (game.pendingBlockRemove.length) {
                    //console.log("Has pending remove");
                    const ctx = getContext("2d")
                    ctx.fillStyle = "black"

                    for (var i = 0; i < game.pendingBlockRemove.length; i++) {
                        //console.log("Remove: " + game.pendingBlockRemove[i]);
                        var b = game.pendingBlockRemove[i];
                        ctx.fillRect(b.x, b.y, b.width, b.height)
                    }
                    game.pendingBlockRemove = []
                }
            }
        }

        Image {
            id: bushLayer
            z: 100
            cache: false
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
        Keys.onPressed: function(event) {
            handleKeyEvent(event, game.qmlTankAction)
        }

        Keys.onReleased: function(event) {
            handleKeyEvent(event, game.qmlTankActionStop)
        }


    }

    Rectangle {
        width: 100
        height: 900
        color: '#444444'

        //anchors.left: battleField.left

        Column {
            anchors.fill: parent
            spacing: 1
            padding: 5

            StartButton {
                text: "1 Player"
                mouseArea.onClicked: game.restart(1);
            }

            StartButton {
                text: "2 Players"
                mouseArea.onClicked: game.restart(2);
            }

            Text {
                width: paintedWidth + 20
                height: paintedHeight + 20
                text: game.lifesStat
                font.pointSize: 12
                color: "white"
                horizontalAlignment: Text.AlignHLeft
                verticalAlignment: Text.AlignVCenter
            }


        }
    }

    SoundEffect {
        id: shotSound
        source: "qrc:///audio/shot"
    }

    SoundEffect {
        id: explNoDamageSound
        source: "qrc:///audio/expl-nodamage"
    }

    SoundEffect {
        id: explBrickSound
        source: "qrc:///audio/expl-brick"
    }

    SoundEffect {
        id: explTankSound
        source: "qrc:///audio/expl-tank"
    }

    SoundEffect {
        id: explFlagSound
        source: "qrc:///audio/expl-flag"
    }

}

