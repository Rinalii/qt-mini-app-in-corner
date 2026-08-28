#include "mrmeow.h"

#include "normalstate.h"
#include "dragstate.h"

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
    : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);

    const QString normal_prefix = "./MyCat_normal_resize5/frame";
    const QString drag_prefix = "./MyCat_drag/frame";
    QString err_msg;
    if(!ReadFrames(normal_prefix, normal_frames_, err_msg)) {
        emit signalErrorHasOccurred(err_msg);
    }

    if(!ReadFrames(drag_prefix, drag_frames_, err_msg)) {
        emit signalErrorHasOccurred(err_msg);
    }

    if (!normal_frames_.isEmpty()) {
        QSize orig_size = normal_frames_.first().size();
        QSize new_size = orig_size / scale;
        //int margin = new_size.height() * 0.5;
        //new_size += QSize(new_size.width() * 0.5, margin);
        resize(new_size);
        setMinimumSize(50, 50);
    }

    MoveToRightBottom();

    // Для ресайза
    grip_ = new QSizeGrip(this);
    grip_->move(width() - grip_->width(), height() - grip_->height());

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


    states_[StateName::Normal] = new NormalState(this, normal_frames_, this);
    states_[StateName::Drag] = new DragState(this, drag_frames_, this);

    SetState(StateName::Normal);
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

void MrMeow::MoveToRightBottom() {
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen) {
        return;
    }
    QRect geom = screen->geometry();
    move(geom.right() - width(), geom.bottom() - height());
}

void MrMeow::SetState(StateName new_state) {

    if(new_state == StateName::Normal) {
        QSize orig_size = normal_frames_.first().size();
        QSize new_size = orig_size / scale;
        resize(new_size);
    } else {
        QSize orig_size = drag_frames_.first().size();
        QSize new_size = orig_size / scale;
        resize(new_size);
    }

    if (current_state_) {
        current_state_->Stop();
    }
    auto it = states_.find(new_state);
    if (it != states_.end()) {
        current_state_ = it->second;
        current_state_->Start();
    }
}

void MrMeow::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);

    QPainter painter(this);
    if (current_state_) {
        current_state_->Render(painter);
    }
}

void MrMeow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        is_left_button_pressed_ = true;
        press_pos_ = event->globalPosition().toPoint();
        drag_started_ = false;

        StopAllPlayers();
        // 10% шанс воспроизвести редкий звук
        double r = QRandomGenerator::global()->generateDouble();

        if (r <= 0.1) {
            rare_player_->play();
        } else {
            left_player_->play();
        }
        event->accept();
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
    if (is_left_button_pressed_ && (event->buttons() & Qt::LeftButton)) {
        if (!drag_started_) {
            QPointF delta = event->globalPosition().toPoint() - press_pos_;
            // Порог в 5 пикселей
            if (delta.manhattanLength() > 5) {
                SetState(StateName::Drag);
                drag_started_ = true;

                DragState* drag_state = dynamic_cast<DragState*>(current_state_);
                if (drag_state) {
                    drag_state->StartDrag(event->globalPosition().toPoint());
                }
            }
        }
    }
    if (current_state_) {
        current_state_->MouseMoved(event);
    }
}

void MrMeow::mouseReleaseEvent(QMouseEvent *event) {
    if (current_state_) {
        current_state_->MouseReleased(event);
    }
    is_left_button_pressed_ = false;
    drag_started_ = false;
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
