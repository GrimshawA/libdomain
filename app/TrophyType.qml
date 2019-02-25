import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Item {
    id: trophies

    property string title: "Default"
    property int unlocked: 11
    property int total: 11
    property var model: null

    height: 100
    width: parent.width

    Text {
        text: title + " (" + unlocked + "/" + total + ")"
    }

    Flow {
        spacing: 2
        width: parent.width
        y: 20

        Repeater {
            model: trophies.model
            delegate: Rectangle {
                width: 10
                height: 10
                color: x_done ? "blue" : "yellow"
            }
        }
    }
}
