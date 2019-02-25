import QtQuick 2.0
import QtQuick.Controls 2.2

Item {
    id: info

    height: parent.height
    width: 220
    state: "none"

    property var viewModel

    Rectangle {
        height: parent.height
        width: info.state != "none" ? 200 : 0

        Behavior on width {
            NumberAnimation {
                duration: 200
                easing.type: Easing.InOutQuad
            }
        }

        Column {
            x: 50
            visible: info.state === "upgrades"

            Text {
                text: "Upgrades"
            }
        }

        Column {
            x: 50
            visible: info.state === "trophies"

            Text {
                text: "Trophies unlocked: " + viewModel.trophies_unlocked + "/" + viewModel.total_trophies
            }

            TrophyType {
                title: "Allegiance Trophies"
                unlocked: viewModel.allegiance_unlocked
                total: viewModel.allegiance_total
                model: viewModel.allegiance_trophies
            }

            TrophyType {
                title: "Misc Trophies"
                unlocked: viewModel.misc_unlocked
                total: viewModel.misc_total
                model: viewModel.misc_trophies
            }
        }
    }

    Rectangle {
        width: 50
        height: 150
        anchors.verticalCenter: parent.verticalCenter

        Column {
            Button {
                width: 50
                height: width
                text: "U"
                onClicked: info.state = "upgrades"
            }

            Button {
                width: 50
                height: width
                text: "T"
                onClicked: info.state = "trophies"
            }

            Button {
                width: 50
                height: width
                text: "E"
                onClicked: info.state = "digs"
            }
        }
    }

}
