import Qt 4.7
import QtDesktop 0.1

Flow {
    anchors.right: parent.right
    Button {
        id: servertest
        height: 23
        width: 75
        text: "Test Server"
        function servertest() {
            mainWindow.servertest()
        }

        onClicked: servertest()
    }
    Button {
        id: create
        height: 23
        width: 50
        text: "Create"
        function create() {

        }

        onClicked: create()
    }
    Button {
        id: login
        height: 23
        width: 50
        text: "Login"
        function login() {
            window.login()
        }

        onClicked: login()
    }
}
