import Qt 4.7

FieldSet {
   anchors {
       left: parent.left
       right: parent.right
    }
    default property alias children: content.children

    Column {
        id: content
        anchors {
            left: parent.left
            right: parent.right
        }
    }
}
