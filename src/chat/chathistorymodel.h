#ifndef CHATHISTORYMODEL_H
#define CHATHISTORYMODEL_H

#include <QAbstractListModel>
#include <QJsonObject>
#include <QList>

#include "chathistory.h"

class ChatHistoryModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        RoleText = Qt::UserRole + 1,
        RoleIsLeft
    };

    explicit ChatHistoryModel(ChatHistory *history, QObject *parent = nullptr);

    // QAbstractListModel overrides
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void slotHistoryChanged();

private:
    ChatHistory *history_;
    QList<ChatHistory::MessageForUI> messages_; // копия для отображения
    void RefreshData();
};

#endif // CHATHISTORYMODEL_H
