#include "renderstate.h"
#include "mrmeow.h"

#include <QTimer>

RenderState::RenderState(MrMeow *context, const QList<QPixmap> &frames, QObject *parent)
    : QObject(parent), context_(context), frames_(frames) {
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &RenderState::slotNextFrame);
}

void RenderState::Start() {
    current_frame_ = 0;
    timer_->start(interval_ms_);
}

void RenderState::Stop() {
    timer_->stop();
}

void RenderState::slotNextFrame() {
    if (frames_.isEmpty()) {
        return;
    }
    current_frame_ = (current_frame_ + 1) % frames_.size();
    if (context_) {
        context_->update();
    }
}
