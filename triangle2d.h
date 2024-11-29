#ifndef TRIANGLE2D_H
#define TRIANGLE2D_H

#include <QPointF>

int sgn(qreal);
bool eq(qreal, qreal);
bool eq(QPointF, QPointF);
QPointF rotatePt(const QPointF&, const QPointF&, const qreal);
QPointF plusPetit(QPointF, QPointF);
qreal calc_angle(QPointF, QPointF, QPointF);

const qreal epsilon = 1;

class Triangle2d
{
public:
    QPointF a, b, c;

    Triangle2d();
    Triangle2d(QPointF, QPointF, QPointF);

    Triangle2d operator +(const QPointF&);
    Triangle2d &operator +=(const QPointF&);
    Triangle2d operator -(const QPointF&);
    Triangle2d &operator -=(const QPointF&);
    Triangle2d operator *(const qreal&);
    Triangle2d operator /(const qreal&);

    QPointF point(const int);
    Triangle2d& rotate(const QPointF&, const qreal);
    bool overlap(const Triangle2d&);
    QPointF centroid();
};

bool li(QPointF, QPointF, QPointF, QPointF);


#endif // TRIANGLE2D_H
