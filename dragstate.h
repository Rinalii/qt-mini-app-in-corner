#ifndef DRAGSTATE_H
#define DRAGSTATE_H

#include "renderstate.h"
#include <QPointer>
#include "movepainter.h"

class DragState : public RenderState {
    Q_OBJECT
public:
    DragState(MrMeow *context, const QList<QPixmap> &frames, QObject *parent = nullptr);

    void Start() override;
    void Stop() override;
    void Render(QPainter &painter) override;

    void MousePressed(QMouseEvent *event) override;
    void MouseMoved(QMouseEvent *event) override;
    void MouseReleased(QMouseEvent *event) override;

private:
    MovePainter move_painter_;
};

#endif // DRAGSTATE_H
