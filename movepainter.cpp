#include "movepainter.h"

#include <QtMath>

MovePainter::MovePainter(QObject *parent) : QObject(parent) {
    physics_timer_ = new QTimer(this);
    connect(physics_timer_, &QTimer::timeout, this, &MovePainter::slotUpdatePhysics);
}

MovePainter::~MovePainter() {
    physics_timer_->stop();
}

void MovePainter::Render(QPainter &painter, const QPixmap &sprite, const QRect &widget_rect) {
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // Точка вращения
    QPointF point_of_rotation(sprite.width() / 3.0, sprite.height() / 20.0);
    L_ = sprite.height()*5.0/8.0;

    // Смещение для центрирования спрайта в виджете
    QPointF offset(
        (widget_rect.width() - sprite.width()) / 2.0,
        (widget_rect.height() - sprite.height()) / 2.0
        );

    QTransform transform;
    transform.translate(offset.x() + point_of_rotation.x(), offset.y() + point_of_rotation.y());
    transform.rotate(-angle_deg_);
    transform.translate(-point_of_rotation.x(), -point_of_rotation.y());

    painter.setTransform(transform);
    painter.drawPixmap(0, 0, sprite);
    painter.setTransform(QTransform());
}

void MovePainter::StartDrag(const QPointF &mousePos) {
    prev_pos_ = mousePos;
    curr_pos_ = mousePos;
    velocity_ = QPointF(0, 0);
    angular_velocity_ = 0.0;
    angle_deg_ = 0.0;
    filtered_accel_ = 0.0;
    physics_timer_->start(16);  // Период 16 мс (60 fps)

    timer_.start();
    last_time_ = timer_.elapsed();
}

void MovePainter::StopDrag() {
    velocity_ = QPointF(0, 0);
    angle_deg_ = 0.0;
    angular_velocity_ = 0.0;
    filtered_accel_ = 0.0;
    physics_timer_->stop();
}

void MovePainter::UpdatePosition(const QPointF &pos) {
    qint64 now = timer_.elapsed();
    qreal t = (now - last_time_);
    if(t < physics_timer_->interval()) {
        curr_pos_ = pos;
    } else {
        prev_pos_ = curr_pos_;
        curr_pos_ = pos;
        last_time_ = now;
    }
}

void MovePainter::slotUpdatePhysics() {
    qreal dt = physics_timer_->interval() / 1000.0; // 0.016 с

    QPointF new_velocity = (curr_pos_ - prev_pos_) / dt;
    if(new_velocity.rx() > max_velocity_) {
        new_velocity = velocity_;
    }
    QPointF raw_accel = (new_velocity - velocity_) / dt;

    // Экспоненциальное скользящее среднее (для устранения шума)
    filtered_accel_ = smooth_factor_ * raw_accel.x() + (1.0 - smooth_factor_) * filtered_accel_;

    UpdateSprite(filtered_accel_, dt);
    velocity_ = new_velocity;
    prev_pos_ = curr_pos_;
    last_time_ = timer_.elapsed();
}

void MovePainter::UpdateSprite(qreal accel, qreal dt) {
    // Текущий угол в радианах
    qreal theta = angle_deg_ * M_PI / 180.0;
    qreal omega = angular_velocity_;

    // Угловое ускорение из уравнения маятника:
    // theta'' = -(g/L)*sin(theta) - (accel/L)*cos(theta)

    // Система уравнений
    // omega = theta'    (угловая скорость)
    // omega' = -(g/L)*sin(theta) - (accel/L)*cos(theta)   (угловое ускорение)

    // Пусть alpha = omega'
    qreal alpha = -(g_ / L_) * sin(theta) - (accel / L_) * cos(theta);

    // Полунеявный метод Эйлера (Метод Эйлера‑Кромера)
    // omega_{n+1} = f_n * h + omega_n
    // обновляем скорость с текущим ускорением
    omega += alpha * dt;
    // применяем затухание
    omega *= damping_;
    // theta_{n+1} = theta_n + omega_{n+1} * dt
    // обновляем угол с новой скоростью
    theta += omega * dt;
    angular_velocity_ = omega;

    angle_deg_ = theta * 180.0 / M_PI;
    if(angle_deg_ > max_angle_) {
        angle_deg_ = max_angle_;
        angular_velocity_ = 0;
    } else if(angle_deg_ < -max_angle_) {
        angle_deg_ = -max_angle_;
        angular_velocity_ = 0;
    }
}
