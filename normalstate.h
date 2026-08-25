#ifndef NORMALSTATE_H
#define NORMALSTATE_H

#include "renderstate.h"
#include <QTimer>

class NormalState : public RenderState
{
    Q_OBJECT
public:
    NormalState(MrMeow *context, const QList<QPixmap> &frames, QObject *parent = nullptr);

    void Render(QPainter &painter) override;
};

#endif // NORMALSTATE_H
