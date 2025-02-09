// Triangle2D avec fonctions
//---------------------------------------------------------
#ifndef TRIANGLE2D_H
#define TRIANGLE2D_H

#include <QPointF>

const qreal epsilon = 1;

QPointF rotatePt(const QPointF&, const QPointF&, const qreal);
qreal distance(const QPointF&, const QPointF&);
qreal calc_angle(QPointF, QPointF, QPointF);
bool eq (qreal, qreal);
bool eq (QPointF, QPointF);
bool li(QPointF, QPointF, QPointF, QPointF);

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

    QList<QPointF> toPolygon();
    QPointF point(const int);
    Triangle2d& rotate(const QPointF&, const qreal);
    bool overlap(const Triangle2d&);
    QPointF centroid();
};


#endif // TRIANGLE2D_H
