import Qt 4.7

Rectangle {
    id: root
    color:"#aaaaaa"
    width: childrenRect.width + borderWidth
    height: childrenRect.height + borderWidth
    radius: borderWidth
    property int borderWidth: 10
    default property alias children: contentholder.children
    Rectangle {
        x: root.borderWidth / 2
        y: root.borderWidth / 2
        color: "#00000000"
        width: childrenRect.width
        height: childrenRect.height
        Column {
                id: contentholder
        }
    }
}
