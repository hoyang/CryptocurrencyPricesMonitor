import QtQuick 2
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls 2
import QtQuick.Controls.Material
import Qt.labs.platform
import QtWebSockets

ApplicationWindow {
    id: mainWindow
    width: 100
    height: 47
    x: Screen.width - width - 20
    y: Screen.height - height - 50
    visible: true
    title: "Cryptocurrency Prices Monitor"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.WA_TranslucentBackground
    color: "transparent"
    
    property string btc_price: "0"
    property string eth_price: "0"

    property variant clickPos: "1, 1"

    function doReconnect() {
        wsClient.active = false
        reconnect.visible = true
        row_btc_price.visible = false
        row_eth_price.visible = false
    }

    Control {
        anchors.fill: parent
        background: Rectangle {
            anchors.fill: parent
            radius: 4
            antialiasing: true
            color: "#4577AF"
            opacity: 0.65
            border.color: "gray"
            border.width: 1
        }

        Button {
            id: reconnect
            z: 1
            visible: false
            anchors.fill: parent
            anchors.margins: 5
            text: "Reconnect"
            onClicked: {
                console.log("Reconnect")
                reconnect.visible = false
                binance.getUserDataStreamLicenseKey()
            }
        }

        RowLayout {
            id: row_btc_price
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.topMargin: 5
            height: 16
            width: 100
            spacing: 0

            Image {
                height: 16
                width: 16
                Layout.leftMargin: 5
                sourceSize.width: width
                sourceSize.height: height
                source: "qrc:/images/btc.png"
                mipmap: true
            }

            Label {
                height: 16
                Layout.fillWidth: true
                text: btc_price
                leftPadding: 5
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 10
                color: "white"
            }
        }

        RowLayout {
            id: row_eth_price
            anchors.left: row_btc_price.left
            anchors.top: row_btc_price.bottom
            anchors.topMargin: 5
            height: 16
            width: 100
            spacing: 0

            Image {
                height: 16
                width: 16
                Layout.leftMargin: 5
                sourceSize.width: width
                sourceSize.height: height
                source: "qrc:/images/eth.png"
                mipmap: true
            }

            Label {
                height: 16
                Layout.fillWidth: true
                text: eth_price
                leftPadding: 5
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 10
                color: "white"
            }
        }
        
        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true

            onPressed: function (mouse) {
                clickPos = Qt.point(mouse.x, mouse.y)
            }

            onPositionChanged: function (mouse) {
                var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
                mainWindow.x += delta.x;
                mainWindow.y += delta.y;
            }
        }
    }

    WebSocket {
        id: wsClient
        active: false

        onTextMessageReceived: function(message) {
            console.log("Received message: " + message)
            var data = JSON.parse(message)

            if(data.e !== undefined) {
                if (data.e == "markPriceUpdate") {
                    if(data.s == "BTCUSDT") {
                        btc_price = Number(data.p).toFixed(2)
                    }

                    if(data.s == "ETHUSDT") {
                        eth_price = Number(data.p).toFixed(2)
                    }
                }
                if (data.e == "listenKeyExpired") {
                    console.log("listenKeyExpired: ", wsClient.active)
                    doReconnect()
                }
            }
        }
        
        onBinaryMessageReceived: function(message) {
            console.log("Received binary message")
        }

        onStatusChanged: function () {
            if (wsClient.status == WebSocket.Error) {
                console.log("Error: ", wsClient.errorString)
            } else if (wsClient.status == WebSocket.Open) {
                var subscribeParam = binance.getSubscribeParam()
                console.log("Subscribe: ", subscribeParam)
                wsClient.sendTextMessage(subscribeParam)
            } else if (wsClient.status == WebSocket.Closed) {
                console.log("Socket closed: ", wsClient.active)
                doReconnect()
            }
        }
    }

    SystemTrayIcon {
        visible: true
        icon.source: "qrc:/images/logo.png"
        tooltip: mainWindow.title

        menu: Menu {
            MenuItem {
                text: "Quit"
                onTriggered: Qt.quit()
            }
        }

        onActivated: function(reason) {
            if(reason == SystemTrayIcon.Trigger) {
                mainWindow.visible = !mainWindow.visible
                if(mainWindow.visible == true) {
                    requestActivate()
                }
            }
        }
    }

    Component.onCompleted: {
        requestActivate()
        console.log("Connecting...")
        binance.getUserDataStreamLicenseKey()
    }

    Connections {
        target: binance

        function onSignalObtainedLicenseKey(ws_endpoint) {
            console.log("Connecting to " + ws_endpoint)
            wsClient.url = ws_endpoint
            wsClient.active = true
            row_btc_price.visible = true
            row_eth_price.visible = true
        }
    }
}