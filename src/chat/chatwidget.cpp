#include "chatwidget.h"
#include <QQmlContext>
#include <QVBoxLayout>

#include "chathistory.h"
#include "aimanager.h"
#include "chatcontroller.h"
#include "chathistorymodel.h"
#include <QQuickWidget>

ChatWidget::ChatWidget(QWidget *parent)
    : QWidget(parent) {

    history_ = new ChatHistory("You are a helpful assistant. Don’t repeat the greeting if you’ve already said hello.", this);
    ai_manager_ = new AIManager("http://localhost:11434", "gemma-4-12b-local", this);
    controller_ = new ChatController(history_, ai_manager_, this);

    // Модель для QML
    model_ = new ChatHistoryModel(history_, this);
    quick_widget_ = new QQuickWidget(this);

    // Передаём C++ объекты в QML контекст
    quick_widget_->rootContext()->setContextProperty("historyModel", model_);
    quick_widget_->rootContext()->setContextProperty("chatController", controller_);
    quick_widget_->setSource(QUrl("qrc:/src/chat/ChatView.qml"));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(quick_widget_);
    setLayout(layout);

    setWindowFlags(Qt::Tool);
    setFixedSize(450, 600);
}
