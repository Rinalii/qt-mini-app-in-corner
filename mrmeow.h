#ifndef MRMEOW_H
#define MRMEOW_H

#include <QWidget>
#include <QPixmap>
#include <QList>
#include <QPoint>
#include <QSizeGrip>
#include <map>

class QPaintEvent;
class QMouseEvent;

class QMediaPlayer;
class QAudioOutput;
class QTimer;

class RenderState;
class NormalState;
class DragState;

class MiniMenu;
class ChatWidget;

class MrMeow : public QWidget
{
    Q_OBJECT
public:
    enum class StateName { Normal, Drag };

    explicit MrMeow(QWidget *parent = nullptr);
    ~MrMeow() = default;

    void SetState(StateName new_state);
    QPoint DragPos() const { return drag_pos_; }
    void SetDragPos(const QPoint &p) { drag_pos_ = p; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

signals:
    void signalErrorHasOccurred(const QString& err_msg);

private slots:
    void slotOpenChat();

private:
    static bool ReadFrames(const QString& resource_prefix, QList<QPixmap>& frames, QString& err_msg);
    bool SetPlayer(const QString& source, QMediaPlayer *&player, QString& err_msg);
    void StopAllPlayers();
    void MoveToRightBottom();

    QList<QPixmap> normal_frames_;
    QList<QPixmap> drag_frames_;
    QPoint drag_pos_;

    // Плееры для звуков
    QMediaPlayer *left_player_ = nullptr;
    QMediaPlayer *right_player_ = nullptr;
    QMediaPlayer *rare_player_ = nullptr;

    // Для ресайза
    QSizeGrip *grip_ = nullptr;
    qreal scale = 6.0;

    RenderState *current_state_ = nullptr;
    std::map<StateName, RenderState*> states_;

    // Для контроля перетаскивания
    bool is_left_button_pressed_ = false;
    QPointF press_pos_;                     // глобальная позиция при нажатии
    bool drag_started_ = false;

    MiniMenu *mini_menu_ = nullptr;
    bool is_double_click_ = false;

    ChatWidget *chat_widget_ = nullptr;
};

#endif // MRMEOW_H
