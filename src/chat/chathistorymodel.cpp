#include "chathistorymodel.h"

#include "chathistory.h"
#include <QDebug>

ChatHistoryModel::ChatHistoryModel(ChatHistory *history, QObject *parent)
    : QAbstractListModel(parent), history_(history) {
    if (history_) {
        connect(history_, &ChatHistory::signalHistoryChanged, this, &ChatHistoryModel::slotHistoryChanged);
        RefreshData();
    }
}

int ChatHistoryModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return messages_.size();
}

QVariant ChatHistoryModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= messages_.size()) {
        return QVariant();
    }

    const ChatHistory::MessageForUI &msg = messages_.at(index.row());
    if (role == RoleText) {
        return msg.text;
    } else if (role == RoleIsLeft) {
        return msg.is_left;
    }
    return QVariant();
}

QHash<int, QByteArray> ChatHistoryModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[RoleText] = "text";
    roles[RoleIsLeft] = "isLeft";
    return roles;
}

void ChatHistoryModel::slotHistoryChanged() {
    RefreshData();
}

void ChatHistoryModel::RefreshData() {
    if (!history_) {
        return;
    }

    // Обновляем модель
    beginResetModel();
    messages_ = history_->GetMessagesForUI(1000);
    endResetModel();
}
