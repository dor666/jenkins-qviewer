import QtQuick 2.5
import QtQuick.Window 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
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
                anchors.left: parent.left
                height: parent.height
                width: parent.width * modelData.buildProgress
                enabled: modelData.running

                SequentialAnimation on color {
                    loops: Animation.Infinite

                    ColorAnimation {
                        from: modelData.color
                        to: Qt.lighter(modelData.color)
                        duration: 1000
                    }
                    ColorAnimation {
                        from: Qt.lighter(modelData.color)
                        to: modelData.color
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
            grid.cellHeight = grid.height / rows
        }

        Component.onCompleted: {
            grid.delegate = delegateComponent
        }
    }

    Connections {
        target: parser
        onNewJobAdded: {
            form.grid.model = parser.jobs
        }
    }
}
