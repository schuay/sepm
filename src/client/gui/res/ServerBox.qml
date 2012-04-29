import Qt 4.7
import QtDesktop 0.1

ColumnFieldSet {
    signal serverTest(string text)
    function serverTestCallBack(result, errMsg){
        if(result){
            serverbubble.visible==true
        }
    }


    id: internalColumn
    anchors {
        left: parent.left
        right: parent.right
    }

    Text {
        text: "Server:"
    }

    TextField {

        height: 24
        id: server
        anchors.right: parent.right
        anchors.left: parent.left
        //KeyNavigation.backtab: username

        onActiveFocusChanged: {
            if(server.text != "" && activeFocus==false){
                mainWindow.testServer(server.text)
            }
            if(serverbubble.visible==true){
                serverbubble.visible=false
            }
        }

        ErrorBubble {
            id: serverbubble
            color:"red"
            text: server.text
        }
    }
    LabeledFileField {
        visible: (server.text || server.activeFocus)
        id: serverCert
        label: "Server Cert:"
    }
}
