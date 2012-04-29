// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Column {
    property alias filepath: fileField.filepath
    property alias label: textLabel.text
    anchors {
        topMargin: 100
        left: parent.left
        right: parent.right
    }


    Text {
        id: textLabel
        font.pointSize: 10
    }

    FileField {
        id: fileField
    }
}
