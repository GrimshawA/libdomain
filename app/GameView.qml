import QtQuick 2.7
import QtQuick.Controls 2.2
import Game 1.0

Item {
    id: gameView

    property GameViewModel viewModel: GameViewModel {}

    Rectangle {
        color: "black"
        anchors.fill: parent
    }

    Image {
        source: "qrc:///BGAlignmentEvil_Vertical.png"
        anchors.fill: parent
    }

    MouseArea {
        anchors.fill: parent
        onClicked: viewModel.click(mouse.x, mouse.y)
    }

    InfoPanel {
        viewModel: gameView.viewModel
    }

    Rectangle {
        color: "black"
        width: 200
        height: 200
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        Column {
            width: 200
            height: parent.height
            spacing: 5

            Repeater {
                model: viewModel.buildings_model
                delegate: Rectangle {
                    color: "blue"
                    width: parent.width
                    height: 40
                    anchors.horizontalCenter: parent.horizontalCenter

                    Text {
                        text: x_name
                        color: "white"
                    }
                }
            }
        }
    }

    Text {
        id: bonus
        property int qty: 0

        text: "Earned: " + qty
        font.pixelSize: 40
        color: "white"

        Connections {
            target: viewModel
            onClicked: {
                bonus.x = x;
                bonus.y = y;
                bonus.qty = 10;
                anim.restart();
            }
        }

        ParallelAnimation {
            id: anim

            NumberAnimation {
                target: bonus
                property: "y"
                duration: 1000
                easing.type: Easing.InOutQuad
                from: bonus.y
                to: bonus.y + 10
            }

            NumberAnimation {
                target: bonus
                property: "opacity"
                duration: 1000
                easing.type: Easing.InOutQuad
                from: 1
                to: 0
            }
        }
    }

    Rectangle {
        color: "black"
        width: parent.width
        height: 50
        anchors.bottom: parent.bottom

        Row {
            spacing: 20
            height: parent.height

            Text {
                text: "Gold: " + viewModel.gold
                color: "white"
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                text: "GoldPerSec: " + viewModel.gold_per_sec
                color: "white"
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                text: "Gems: " + viewModel.gems
                color: "white"
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                text: "Rubies: " + viewModel.rubies
                color: "white"
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                text: "Assistants: " + viewModel.assistants
                color: "white"
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Button {
            text: "Quit"
            anchors.right: parent.right
        }

    }
}
