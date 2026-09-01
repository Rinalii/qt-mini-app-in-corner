#ifndef CHATHISTORY_H
#define CHATHISTORY_H

#include <QObject>
#include <QJsonArray>
#include <QString>

class ChatHistory : public QObject
{
    Q_OBJECT
public:
    explicit ChatHistory(const QString &system_prompt = "", QObject *parent = nullptr);

    // Добавить сообщение (role: "system", "user" или "assistant")
    void AddMessage(const QString &role, const QString &content);
    void SetSystemPrompt(const QString &prompt);

    QJsonArray GetMessagesForRequest(int max_count) const;

    struct MessageForUI {
        QString text;
        bool is_left;
    };
    QList<MessageForUI> GetMessagesForUI(int max_count) const;

    int getSize() const;
    void Clear();

signals:
    void signalHistoryChanged();

private:
    QList<QJsonObject> messages_;
    QString system_prompt_;
};

#endif // CHATHISTORY_H
