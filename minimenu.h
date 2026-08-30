#ifndef MINIMENU_H
#define MINIMENU_H

#include <QWidget>

class QPushButton;

class MiniMenu : public QWidget
{
    Q_OBJECT
public:
    explicit MiniMenu(QWidget *parent = nullptr);

signals:
    void signalOpenChat();

private:
    QPushButton* open_chat_btn_;
    QPushButton* exit_btn_;
};

#endif // MINIMENU_H
