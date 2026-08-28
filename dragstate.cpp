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

void DragState::StartDrag(const QPointF &mouse_global) {
    if (!context_ || frames_.isEmpty()) {
        return;
    }

    QPixmap frame = frames_.first();
    QPixmap scaled = frame.scaled(context_->size() * 2.0 / 3.0,
                                  Qt::KeepAspectRatio,
                                  Qt::SmoothTransformation);

    // Точка вращения внутри спрайта
    QPointF pr(scaled.width() / 3.0, scaled.height() / 20.0);
    QPointF offset((context_->width() - scaled.width()) / 2.0,
                   (context_->height() - scaled.height()) / 2.0);

    // Перемещаем виджет, чтобы точка вращения оказалась под курсором
    context_->move((mouse_global - offset - pr).toPoint());

    // Запоминаем смещение от левого верхнего угла виджета до точки вращения
    context_->SetDragPos((offset + pr).toPoint());  // для mouseMoveEvent

    move_painter_.StartDrag(mouse_global);
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

void DragState::MouseReleased(QMouseEvent *event) {     //TODO: Избавиться от магических чисел и почистить логику смещения спрайта
    if (!context_) {
        return;
    }
    QPixmap frame = frames_.first();
    QPixmap scaled = frame.scaled(context_->size() * 2.0 / 3.0,
                                  Qt::KeepAspectRatio,
                                  Qt::SmoothTransformation);
    QPointF offset((context_->width() - scaled.width()) / 2.0,
                   (context_->height() - scaled.height()) / 2.0);
    QPointF mouseGlobal = event->globalPosition().toPoint();

    QPointF right_bottom{static_cast<qreal>(scaled.width()), static_cast<qreal>(scaled.height())};
    QPointF normal_size{static_cast<qreal>(200), static_cast<qreal>(134)};
    QPointF right_pos(mouseGlobal.x() - offset.x()-scaled.width()/3.0 + normal_size.x()/3.0,
                      mouseGlobal.y()+right_bottom.y() - 2*offset.y() - normal_size.y());

    context_->move(right_pos.toPoint());
    if (event->button() == Qt::LeftButton) {
        move_painter_.StopDrag();
        context_->SetState(MrMeow::StateName::Normal);
    }
}
