#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>

class ChatHistory;
class AIManager;
class ChatController;
class ChatHistoryModel;
class QQuickWidget;

class ChatWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChatWidget(QWidget *parent = nullptr);

private:
    // Логика
    ChatHistory *history_;
    AIManager *ai_manager_;
    ChatController *controller_;

    // Отображение
    ChatHistoryModel *model_;
    QQuickWidget *quick_widget_;
};

#endif // CHATWIDGET_H
