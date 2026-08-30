#include "minimenu.h"

#include <QPushButton>
#include <QVBoxLayout>

#include <QApplication>

MiniMenu::MiniMenu(QWidget *parent)
    : QWidget{parent} {

    setFixedSize(120, 80);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    open_chat_btn_ = new QPushButton("Открыть чат", this);
    exit_btn_ = new QPushButton("Выход", this);

    QVBoxLayout* v_layout = new QVBoxLayout(this);
    v_layout->addWidget(open_chat_btn_);
    v_layout->addWidget(exit_btn_);

    setLayout(v_layout);

    connect(exit_btn_, &QPushButton::clicked, qApp, &QApplication::quit);
    connect(open_chat_btn_, &QPushButton::clicked, this, &MiniMenu::signalOpenChat);
}
