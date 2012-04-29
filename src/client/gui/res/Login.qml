import Qt 4.7
import QtDesktop 0.1

ApplicationWindow {
    signal login()
    signal register()

    id: mainWindow
    width: 180
    height: 300

    Column {
        spacing: 5
        anchors {
            fill: parent
            right: parent.right
            left: parent.left
            margins: 5
        }

        ServerBox {
           id: serverBox
        }

        UserBox {
           id: userBox
        }

        ButtonBox {
           id: buttonBox
        }
    }
}
