#include "dragstate.h"

#include "mrmeow.h"
#include "movepainter.h"
#include <QMouseEvent>

DragState::DragState(MrMeow *context, const QList<QPixmap> &frames, QObject *parent)
    : RenderState(context, frames, parent), move_painter_(this) {
    interval_ms_ = 40;
}

void DragState::Start() {
    RenderState::Start();
    move_painter_.StartDrag(drag_start_pos_);
}

void DragState::Stop() {
    RenderState::Stop();
    move_painter_.StopDrag();
}

void DragState::Render(QPainter &painter) {
    if (frames_.isEmpty() || !context_) {
        return;
    }
    QPixmap frame = frames_[current_frame_];
    QPixmap scaled = frame.scaled(context_->size() * 2.0 / 3.0,
                                  Qt::KeepAspectRatio,
                                  Qt::SmoothTransformation);
    move_painter_.Render(painter, scaled, context_->rect());
}

void DragState::MousePressed(QMouseEvent *event) {
    if (!context_) {
        return;
    }
    if (event->button() == Qt::LeftButton) {
        drag_start_pos_ = event->globalPosition().toPoint() - context_->pos();
        move_painter_.StartDrag(drag_start_pos_);
    }
}

void DragState::MouseMoved(QMouseEvent *event) {
    if (!context_) {
        return;
    }
    if (event->buttons() & Qt::LeftButton) {
        context_->move(event->globalPosition().toPoint() - context_->DragPos());
        move_painter_.UpdatePosition(event->globalPosition().toPoint());
    }
}

void DragState::MouseReleased(QMouseEvent *event) {
    if (!context_) {
        return;
    }
    if (event->button() == Qt::LeftButton) {
        move_painter_.StopDrag();
        context_->SetState(MrMeow::StateName::Normal);
    }
}
