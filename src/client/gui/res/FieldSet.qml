import Qt 4.7

Rectangle {
        id: root
        color:"#aaaaaa"
        property int borderWidth: 10
        default property alias children: content.children

        width: content.childrenRect.width + borderWidth * 2
        height: content.childrenRect.height + borderWidth * 2
        radius: borderWidth

        Rectangle {
            id: content
            color: "#00000000"
            anchors {
                fill: parent
                margins: root.borderWidth
            }
        }
}
