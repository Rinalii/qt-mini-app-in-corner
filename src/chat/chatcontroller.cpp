#include "chatcontroller.h"
#include <QJsonArray>

#include "chathistory.h"
#include "aimanager.h"

ChatController::ChatController(ChatHistory *history, AIManager *ai_manager, QObject *parent)
    : QObject(parent)
    , history_(history)
    , ai_manager_(ai_manager) {
    connect(ai_manager_, &AIManager::signalChunkReceived, this, &ChatController::slotChunkReceived);
    connect(ai_manager_, &AIManager::signalResponseFinished, this, &ChatController::slotResponseFinished);
    connect(ai_manager_, &AIManager::signalErrorOccurred, this, &ChatController::slotErrorOccurred);
}

QString ChatController::GetCurrAssistantReply() const {
    return curr_reply_;
}

bool ChatController::IsWaitingForResponse() const {
    return is_waiting_;
}

void ChatController::SetCurrReply(const QString &text) {
    if (curr_reply_ == text) {
        return;
    }
    curr_reply_ = text;
    emit signalCurrAssistantReplyChanged();
}

void ChatController::AddMessage(const QString &role, const QString &content) {
    if (!content.isEmpty()) {
        history_->AddMessage(role, content);
    }

    SetCurrReply("");
    is_waiting_ = role == "user";
    emit signalWaitingForResponseChanged();
}

void ChatController::slotSendUserMessage(const QString &text) {
    QString user_trimmed = text.trimmed();
    if (user_trimmed.isEmpty() || is_waiting_) {
        return;
    }

    AddMessage("user", user_trimmed);

    // Берём последние 20 сообщений
    QJsonArray recent = history_->GetMessagesForRequest(20);
    ai_manager_->SendRequest(recent);
}

void ChatController::slotChunkReceived(const QString &chunk) {
    curr_reply_ += chunk;
    emit signalCurrAssistantReplyChanged();
}

void ChatController::slotResponseFinished(const QString &full_text) {
    QString final_answer = full_text.isEmpty() ? curr_reply_ : full_text;
    AddMessage("assistant", final_answer);
}

void ChatController::slotErrorOccurred(const QString &error) {
    AddMessage("assistant", "Ошибка: " + error);
}
