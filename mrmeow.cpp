#include "mrmeow.h"

#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QRandomGenerator>
#include <QUrl>
#include <QResizeEvent>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>

MrMeow::MrMeow(QWidget *parent)
    : QWidget(parent), curr_frame_(0)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);

    const QString resource_prefix = "./MyCat4/frame";
    QString err_msg;
    if(!ReadFrames(resource_prefix, simp_frames_, err_msg)) {
        emit signalErrorHasOccurred(err_msg);
    }

    QSize orig_size = simp_frames_.first().size();
    QSize new_size = orig_size / 6;
    resize(new_size);
    setMinimumSize(50, 50);

    MoveToRightBottom();

    // Таймер для смены кадров
    frame_timer_ = new QTimer(this);
    connect(frame_timer_, &QTimer::timeout, this, &MrMeow::slotNextFrame);
    frame_timer_->start(50);

    // Инициализация звуков
    if(!SetPlayer("./sounds/meow.wav", left_player_, err_msg)) {
        emit signalErrorHasOccurred(err_msg);
    }
    if(!SetPlayer("./sounds/song.wav", right_player_, err_msg)) {
        emit signalErrorHasOccurred(err_msg);
    }
    if(!SetPlayer("./sounds/meow2.wav", rare_player_, err_msg)) {
        emit signalErrorHasOccurred(err_msg);
    }

    // Обработка ошибок загрузки звуков
    connect(left_player_, &QMediaPlayer::errorOccurred, [this](QMediaPlayer::Error error) {
        qDebug() << "Left player error:" << error << left_player_->errorString();
    });
    connect(right_player_, &QMediaPlayer::errorOccurred, [this](QMediaPlayer::Error error) {
        qDebug() << "Right player error:" << error << right_player_->errorString();
    });
    connect(rare_player_, &QMediaPlayer::errorOccurred, [this](QMediaPlayer::Error error) {
        qDebug() << "Rare player error:" << error << rare_player_->errorString();
    });

    // Для ресайза
    grip_ = new QSizeGrip(this);
    grip_->move(width() - grip_->width(), height() - grip_->height());  // Начальная позиция
}

bool MrMeow::ReadFrames(const QString &resource_prefix, QList<QPixmap> &frames, QString &err_msg) {
    err_msg.clear();
    if(resource_prefix.isEmpty()) {
        err_msg = "resource_prefix пуст.";
        return false;
    }
    int idx = 0;
    while (true) {
        QString filename = resource_prefix + QString("%1").arg(idx, 4, 10, QChar('0')) + ".png";
        QPixmap pix;
        if (pix.load(filename)) {
            frames.append(pix);
            ++idx;
        } else {
            break;
        }
    }

    if (frames.isEmpty()) {
        err_msg = "Не найдено ни одного кадра в ресурсах ("+ resource_prefix +", ...). Выход.";
        QTimer::singleShot(0, QApplication::instance(), &QApplication::quit);
        return false;
    }
    return true;
}

bool MrMeow::SetPlayer(const QString &source, QMediaPlayer *&player, QString &err_msg) {
    err_msg.clear();
    if(source.isEmpty()) {
        err_msg = "source пуст.";
        return false;
    }
    if(player != nullptr) {
        err_msg = "player уже существует.";
        return false;
    }
    player = new QMediaPlayer(this);
    QAudioOutput *audio_output = new QAudioOutput(this);
    audio_output->setVolume(0.5);
    player->setAudioOutput(audio_output);
    player->setSource(QUrl(source));
    return true;
}

void MrMeow::StopAllPlayers() {
    if(!left_player_ || !right_player_ || !rare_player_) {
        return;
    }
    if (left_player_->isPlaying()) {
        left_player_->stop();
        left_player_->setPosition(0);
    }
    if (right_player_->isPlaying()) {
        right_player_->stop();
        right_player_->setPosition(0);
    }
    if (rare_player_->isPlaying()) {
        rare_player_->stop();
        rare_player_->setPosition(0);
    }
}

void MrMeow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    if (grip_) {
        grip_->move(width() - grip_->width(), height() - grip_->height());
    }
}

void MrMeow::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    if (grip_) {
        grip_->move(width() - grip_->width(), height() - grip_->height());
    }
}

void MrMeow::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);

    if (curr_frame_ >= simp_frames_.size()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QPixmap pix = simp_frames_[curr_frame_];
    QPixmap scaled = pix.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    painter.drawPixmap(QPoint(0, 0), scaled);
}

void MrMeow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        drag_pos_ = event->globalPosition().toPoint() - pos();
        event->accept();
        StopAllPlayers();

        // 10% шанс воспроизвести редкий звук
        double r = QRandomGenerator::global()->generateDouble();

        if (r <= 0.1) {
            rare_player_->play();
        } else {
            left_player_->play();
        }
    } else if (event->button() == Qt::RightButton) {
        event->accept();
        StopAllPlayers();
        right_player_->play();
    } else if (event->button() == Qt::MiddleButton) {
        StopAllPlayers();
        QApplication::quit();
    }
}

void MrMeow::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - drag_pos_);
        event->accept();
    }
}

void MrMeow::slotNextFrame() {
    if (simp_frames_.isEmpty()) {
        return;
    }
    curr_frame_ = (curr_frame_ + 1) % simp_frames_.size();
    update();
}

void MrMeow::MoveToRightBottom() {
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) {
        return;
    }
    QRect geom = screen->geometry();
    move(geom.right() - width(), geom.bottom() - height());
}
