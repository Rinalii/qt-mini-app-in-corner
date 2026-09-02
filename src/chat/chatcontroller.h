#ifndef CHATCONTROLLER_H
#define CHATCONTROLLER_H

#include <QObject>
#include <QString>

class ChatHistory;
class AIManager;

class ChatController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentAssistantReply READ GetCurrAssistantReply NOTIFY signalCurrAssistantReplyChanged) // Для QML
    Q_PROPERTY(bool isWaitingForResponse READ IsWaitingForResponse NOTIFY signalWaitingForResponseChanged)      // Для QML

public:
    explicit ChatController(ChatHistory *history, AIManager *ai_manager, QObject *parent = nullptr);

    QString GetCurrAssistantReply() const;
    bool IsWaitingForResponse() const;

public slots:
    void slotSendUserMessage(const QString &text);

signals:
    void signalCurrAssistantReplyChanged();
    void signalWaitingForResponseChanged();

private slots:
    void slotChunkReceived(const QString &chunk);
    void slotResponseFinished(const QString &full_text);
    void slotErrorOccurred(const QString &error);

private:
    void SetCurrReply(const QString &text);
    void AddMessage(const QString &role, const QString &content);

private:
    ChatHistory *history_;
    AIManager *ai_manager_;
    QString curr_reply_;
    bool is_waiting_ = false;
};

#endif // CHATCONTROLLER_H
