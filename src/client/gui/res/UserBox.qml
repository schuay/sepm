import Qt 4.7
import QtDesktop 0.1

ColumnFieldSet {

    anchors {
        left: parent.left
        right: parent.right
    }

    Text {
        text: "Username:"
    }

    TextField {
        height: 24
        id: username
        anchors.right: parent.right
        anchors.left: parent.left
    }


    Text {
        text: " "
        font.pixelSize: 12
    }
    LabeledFileField {
        id: publicssh
        label: "Own Public Key:"

    }
    LabeledFileField {
        id: privatessh
        label: "Own Private Key:"
    }

    Text {
        text: " Don't have a key?"
        color: "blue"
        font.underline: true
        font.pixelSize: 10
    }
    Text {
        text: " "
        font.pixelSize: 3
    }
}
