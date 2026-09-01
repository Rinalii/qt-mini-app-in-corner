#include "chathistory.h"

#include <QJsonObject>

ChatHistory::ChatHistory(const QString &system_prompt, QObject *parent)
    : QObject(parent)
    , system_prompt_(system_prompt) {
}

void ChatHistory::AddMessage(const QString &role, const QString &content) {
    if (role == "system") {
        SetSystemPrompt(content);
        return;
    }
    QJsonObject msg;
    msg["role"] = role;
    msg["content"] = content;
    messages_.append(msg);
    emit signalHistoryChanged();
}

void ChatHistory::SetSystemPrompt(const QString &prompt) {
    system_prompt_ = prompt;
    emit signalHistoryChanged();
}

QJsonArray ChatHistory::GetMessagesForRequest(int max_count) const {
    QJsonArray result;

    // Всегда добавляем системное сообщение, если оно задано
    if (!system_prompt_.isEmpty()) {
        QJsonObject sys;
        sys["role"] = "system";
        sys["content"] = system_prompt_;
        result.append(sys);
    }

    // Сколько ещё сообщений можно взять из истории
    int remaining = max_count - result.size();
    if (remaining <= 0)
        return result;

    int total = messages_.size();
    int start = qMax(0, total - remaining);
    for (int i = start; i < total; ++i) {
        result.append(messages_.at(i));
    }
    return result;
}

QList<ChatHistory::MessageForUI> ChatHistory::GetMessagesForUI(int max_count) const {
    QList<MessageForUI> result;

    for (int i = 0; i < messages_.size() && i < max_count; ++i) {
        QJsonObject message = messages_.at(i);
        MessageForUI message_for_ui;
        message_for_ui.text = message["content"].toString();
        message_for_ui.is_left = message["role"].toString() == "assistant";
        result.append(message_for_ui);
    }
    return result;
}

int ChatHistory::getSize() const {
    return messages_.size() + (system_prompt_.isEmpty() ? 0 : 1);
}

void ChatHistory::Clear() {
    messages_.clear();
    emit signalHistoryChanged();
}
