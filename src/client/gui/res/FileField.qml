import Qt 4.7
import QtDesktop 0.1



Rectangle {
    property alias filepath: filepath.text
    height: 25
    color: "transparent"

    anchors {
        left: parent.left
        right: parent.right
    }
    TextField {
        id: filepath
        height: parent.height
        anchors {
            left: parent.left
            right: fileChooserButton.left
        }
    }
    Button {
        text: "..."
        id: fileChooserButton
        height: parent.height
        width: fileChooserButton.height
        anchors {
           right: parent.right
        }
        onClicked: {
            var filename=context.fileDialog()
            if(filename!=null){
                filepath.text=filename
            }
        }
    }
}
