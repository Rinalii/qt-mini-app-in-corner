#include "normalstate.h"

#include "mrmeow.h"

#include <QPainter>

NormalState::NormalState(MrMeow *context, const QList<QPixmap> &frames, QObject *parent)
    : RenderState(context, frames, parent) {
    interval_ms_ = 40;
}

void NormalState::Render(QPainter &painter) {
    if (frames_.isEmpty() || !context_) {
        return;
    }
    QPixmap frame = frames_[current_frame_];
    QPixmap scaled = frame.scaled(context_->size() * 2.0 / 3.0,
                                  Qt::KeepAspectRatio,
                                  Qt::SmoothTransformation);
    painter.drawPixmap(0, 0, scaled);
}
