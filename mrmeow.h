#ifndef MRMEOW_H
#define MRMEOW_H

#include <QWidget>
#include <QPixmap>
#include <QList>
#include <QPoint>
#include <QSizeGrip>

class QPaintEvent;
class QMouseEvent;

class QMediaPlayer;
class QAudioOutput;
class QTimer;

class MrMeow : public QWidget
{
    Q_OBJECT
public:
    explicit MrMeow(QWidget *parent = nullptr);
    ~MrMeow() = default;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

signals:
    void signalErrorHasOccurred(const QString& err_msg);

private slots:
    void slotNextFrame();

private:
    static bool ReadFrames(const QString& resource_prefix, QList<QPixmap>& frames, QString& err_msg);
    bool SetPlayer(const QString& source, QMediaPlayer *&player, QString& err_msg);
    void StopAllPlayers();
    void MoveToRightBottom();

    QList<QPixmap> simp_frames_;
    int curr_frame_;
    QTimer *frame_timer_ = nullptr;
    QPoint drag_pos_;

    // Плееры для звуков
    QMediaPlayer *left_player_ = nullptr;
    QMediaPlayer *right_player_ = nullptr;
    QMediaPlayer *rare_player_ = nullptr;

    // Для ресайза
    QSizeGrip *grip_ = nullptr;
};

#endif // MRMEOW_H
