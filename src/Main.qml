import QtQuick 2.3
import QtQuick.Window 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import "."


ApplicationWindow {
    visible: true
    width: 800
    height: 600

    Component {
        id: delegateComponent
        Rectangle {
            color: modelData.color
            width: form.grid.cellWidth - form.grid.anchors.leftMargin
            height: form.grid.cellHeight - form.grid.anchors.topMargin
            radius: 5

            Rectangle {
                id: animatedRectangle
                property color fromColor : parent.color
                property color toColor : Qt.lighter(parent.color)

                anchors.left: parent.left
                height: parent.height
                width: parent.width * modelData.buildProgress
                enabled: modelData.running

                SequentialAnimation on color {
                    loops: Animation.Infinite
                    running: modelData.running

                    ColorAnimation {
                        from: animatedRectangle.fromColor
                        to: animatedRectangle.toColor
                        duration: 1000
                    }
                    ColorAnimation {
                        from: animatedRectangle.toColor
                        to: animatedRectangle.fromColor
                        duration: 1000
                    }
                }
            }

            Text {
                text: modelData.objectName
                color: "white"
                anchors.centerIn: parent
            }
        }
    }

    MainForm {
        id: form
        anchors.fill: parent

        grid.delegate: delegateComponent

        onWidthChanged: {
            var columns = 3 //!@todo temp
            grid.cellWidth = grid.width / columns
            var rows = grid.count / columns
            if(grid.count % columns)
                ++rows
            grid.cellHeight = grid.height / rows
        }

        Component.onCompleted: {
            grid.delegate = delegateComponent
        }
    }

    Timer {
        id: deferJobListChanged
        interval: 1; running: false; repeat: false
        onTriggered: {
            console.log("New job(s) added, refreshing list")
            form.grid.model = parser.jobs
            form.onWidthChanged()
        }
    }
    Connections {
        target: parser
        onNewJobAdded: deferJobListChanged.start()
    }
}
