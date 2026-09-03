import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        color: "#1a1a1a"
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        ListView {
            id: chatListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8
            model: historyModel

            delegate: ChatBubble {
                text: model.text
                isLeft: model.isLeft
                maxWidth: 2*parent.width/3
                anchors.left: isLeft ? parent.left : undefined
                anchors.right: isLeft ? undefined : parent.right
                anchors.leftMargin: isLeft ? 10 : 0
                anchors.rightMargin: isLeft ? 0 : 10

                bubbleColor: isLeft ? "#fff485" : "#FFB740"
            }
            onCountChanged: positionViewAtEnd()
        }

        Text {
            id: typingIndicator
            visible: chatController.isWaitingForResponse && chatController.currentAssistantReply === ""
            text: "·"
            font.pointSize: 20
            color: "gray"
            Timer {
                id: dotTimer
                interval: 400
                running: parent.visible
                repeat: true
                onTriggered: {
                    var dots = parent.text
                    if (dots === "·") parent.text = "··"
                    else if (dots === "··") parent.text = "···"
                    else parent.text = "·"
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            TextField {
                id: inputField
                Layout.fillWidth: true
                placeholderText: "Введите вопрос..."
                onAccepted: sendMessage()
                enabled: !chatController.isWaitingForResponse
                color: "#ffffff"
                background: Rectangle {
                    color: "#404040"
                    radius: 4
                    border.width: 0
                }
            }
            Button {
                text: "Отправить"
                enabled: !chatController.isWaitingForResponse && inputField.text.trim() !== ""
                onClicked: sendMessage()
                background: Rectangle {
                    color: enabled ? "#FFB740" : "#404040"
                    radius: 4
                }
            }
        }
    }

    function sendMessage() {
        if (inputField.text.trim() === "") return
        chatController.slotSendUserMessage(inputField.text.trim())
        inputField.clear()
    }
}
