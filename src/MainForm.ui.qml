import QtQuick 2.4

Item {
    width: 400
    height: 400
    property alias grid: grid

    Rectangle {
        id: rectangle1
        color: "#000000"
        border.width: 0
        border.color: "#ffffff"
        anchors.fill: parent

        GridView {
            id: grid
            clip: true
            anchors.fill: parent
            anchors.leftMargin: 5
            anchors.topMargin: 5
            model: ListModel {}
            delegate: Item {
                x: 5
                height: 50
                Column {
                    Rectangle {
                        width: 40
                        height: 40
                        color: colorCode
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    Text {
                        x: 5
                        text: name
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.bold: true
                    }
                    spacing: 5
                }
            }
            cellHeight: 70
            cellWidth: 200
        }
    }
}
