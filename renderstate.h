#ifndef RENDERSTATE_H
#define RENDERSTATE_H

#include <QObject>
#include <QPointer>
#include <QPainter>
#include <QMouseEvent>

class MrMeow;

class RenderState : public QObject
{
    Q_OBJECT
public:
    RenderState(MrMeow *context, const QList<QPixmap> &frames, QObject *parent = nullptr);

    virtual ~RenderState() = default;

    virtual void Start();
    virtual void Stop();
    virtual void Render(QPainter &painter) = 0;

    virtual void MousePressed(QMouseEvent *event) { Q_UNUSED(event); }
    virtual void MouseMoved(QMouseEvent *event) { Q_UNUSED(event); }
    virtual void MouseReleased(QMouseEvent *event) { Q_UNUSED(event); }

protected slots:
    virtual void slotNextFrame();

protected:
    QPointer<MrMeow> context_;
    QList<QPixmap> frames_;
    int current_frame_ = 0;
    QTimer *timer_ = nullptr;
    int interval_ms_ = 40; // 25 fps
};

#endif // RENDERSTATE_H
