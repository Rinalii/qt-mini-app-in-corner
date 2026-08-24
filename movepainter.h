#ifndef MOVEPAINTER_H
#define MOVEPAINTER_H

#include <QElapsedTimer>
#include <QPainter>
#include <QTimer>

class MovePainter : public QObject
{
    Q_OBJECT
public:
    MovePainter(QObject *parent = nullptr);
    ~MovePainter();

    void Render(QPainter& painter, const QPixmap& sprite, const QRect& widget_rect);
    void StartDrag(const QPointF &mousePos);
    void StopDrag();
    void UpdatePosition(const QPointF &pos);

private slots:
    void slotUpdatePhysics();

private:
    void UpdateSprite(qreal accel, qreal dt);

private:
    QTimer *physics_timer_;
    QPointF velocity_;               // пикс/с
    QPointF prev_pos_;
    QPointF curr_pos_;

    QElapsedTimer timer_;
    qint64 last_time_;

    qreal angle_deg_ = 0.0;          // угол наклона спрайта (градусы)
    qreal angular_velocity_ = 0.0;   // угловая скорость (рад/с)
    qreal filtered_accel_ = 0.0;     // сглаженное ускорение (пикс/с^2)

    qreal L_ = 150.0;
    static constexpr qreal max_angle_ = 45.0;
    static constexpr qreal max_velocity_ = 2000.0;
    static constexpr qreal smooth_factor_ = 0.3;    // Для EMA
    static constexpr qreal g_ = 2 * 980.0;
    static constexpr qreal damping_ = 0.99;         // коэффициент затухания скорости
};

#endif // MOVEPAINTER_H
