import QtQuick 2.15
import QtQuick.Shapes

Rectangle {
    id: bubble

    property string name: "ChatBubble"
    property alias text: bubble_txt.text

    property color bubbleColor: "#fff485"
    property bool isLeft: true
    property int maxWidth: 400
    property int cornerRadius: 15
    property int padding: cornerRadius/2

    anchors.left: isLeft ? parent.left : undefined
    anchors.right: isLeft ? undefined : parent.right
    anchors.leftMargin: isLeft ? 10 : 0
    anchors.rightMargin: isLeft ? 0 : 10

    color: "transparent"
    width: Math.min(maxWidth, Math.max(cornerRadius*2, bubble_txt.implicitWidth + padding * 2))
    height: bubble_txt.height < cornerRadius*2 ? cornerRadius*2 : bubble_txt.height + padding*2

    Shape {
        anchors.fill: bubble
        layer.enabled: true
        layer.samples: 8

        transform: Scale {
            xScale: bubble.isLeft ? 1 : -1
            origin.x: bubble.width / 2
        }

        ShapePath {
            fillColor: bubble.bubbleColor
            strokeWidth: 0

            startX: cornerRadius
            startY: 0

            PathLine { x: bubble.width - cornerRadius; y: 0 }
            PathArc { x: bubble.width; y: cornerRadius; radiusX: cornerRadius; radiusY: cornerRadius }
            PathLine { x: bubble.width; y: bubble.height - cornerRadius }
            PathArc { x: bubble.width - cornerRadius; y: bubble.height; radiusX: cornerRadius; radiusY: cornerRadius }
            PathLine { x: cornerRadius; y: bubble.height }

            PathArc { x: bubble.padding; y: bubble.height - bubble.padding/3; radiusX: cornerRadius; radiusY: cornerRadius }
            PathLine { x: 0; y: bubble.height }
            PathLine { x: bubble.padding/3; y: bubble.height - bubble.padding}
            PathArc { x: 0; y: bubble.height - cornerRadius; radiusX: cornerRadius; radiusY: cornerRadius }

            PathLine { x: 0; y: cornerRadius }
            PathArc { x: cornerRadius; y: 0; radiusX: cornerRadius; radiusY: cornerRadius }
        }
    }
    TextEdit {
        id: bubble_txt
        x: bubble.padding
        y: (bubble.height - height) / 2
        readOnly: true
        color: "black"
        font.family: "Yu Gothic UI"
        font.pointSize: 12
        width: bubble.width - bubble.padding * 2
        wrapMode: Text.WordWrap
        selectByMouse: true
    }
}
