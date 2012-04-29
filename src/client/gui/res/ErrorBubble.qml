import Qt 4.7

Rectangle {
    visible: errorText.text
    anchors.right: parent.right
    color: "green"
    radius: 10
    smooth: true
    width: 70
    height: 20
    y: -10
    property alias text: errorText.text
    Text {
        id: errorText
        height: 12
        anchors {
            top: parent.baseline
            horizontalCenter: parent.horizontalCenter
        }
    }
}
