#ifndef TRIANGLE2D_H
#define TRIANGLE2D_H

#include <QPointF>

bool eq(qreal a, qreal b);
bool eq(QPointF a, QPointF b);
QPointF rotatePt(const QPointF& v, const QPointF& c, const qreal a);
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
