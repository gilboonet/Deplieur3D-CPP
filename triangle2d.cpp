// Triangle2D avec fonctions
//---------------------------------------------------------
#include "triangle2d.h"

#include <QtMath>
#include <QList>
//---------------------------------------------------------
QPointF rotatePt(const QPointF& v, const QPointF& c, const qreal a) {
    qreal lcos = cos(a);
    qreal lsin = sin(a);

    QPointF r = QPointF(
        (lcos *(v.x() - c.x())) +(lsin *(v.y() - c.y())) + c.x(),
        (lcos *(v.y() - c.y())) -(lsin *(v.x() - c.x())) + c.y()
        );
    return r;
}


qreal distance(const QPointF& v1, const QPointF& v2) {
    QPointF d = v2 - v1;
    return sqrt((d.x() * d.x()) +(d.y() * d.y()));
}

qreal calc_angle(QPointF a, QPointF b, QPointF c) {
    QPointF ab = b - a;
    QPointF ac = c - a;

    qreal rot_ab_ac = atan2(ac.y() * ab.x() - ac.x() * ab.y(), ac.x() * ab.x() + ac.y() * ab.y());
    return qRadiansToDegrees(rot_ab_ac);
}

QList<QPointF> Triangle2d::toPolygon() {
    return {a, b, c};
}

bool eq (qreal a, qreal b) {
    return qFabs(a - b) < epsilon;
}

bool eq (QPointF a, QPointF b) {
    return eq(a.x(), b.x()) && eq(a.y(), b.y());
}

bool li(QPointF l1S, QPointF l1E, QPointF l2S, QPointF l2E) {
    // true if the lines intersect
    if (eq(l1S, l2S) || eq(l1S, l2E) || eq(l1E, l2S) || eq(l1E, l2E)) {
        return 0;
    }

    qreal denominator =((l2E.y() - l2S.y()) *(l1E.x() - l1S.x()))
                        -((l2E.x() - l2S.x()) *(l1E.y() - l1S.y()));

    if(denominator == 0)
        return 0;

    qreal
        a = l1S.y() - l2S.y(),
        b = l1S.x() - l2S.x(),
        numerator1 =((l2E.x() - l2S.x()) * a) -((l2E.y() - l2S.y()) * b),
        numerator2 =((l1E.x() - l1S.x()) * a) -((l1E.y() - l1S.y()) * b);
    a = numerator1 / denominator;
    b = numerator2 / denominator;

    if((a > 0) &&(a < 1) &&(b > 0) &&(b < 1))
        return 1;
    else
        return 0;
}
//---------------------------------------------------------
Triangle2d::Triangle2d() {}
Triangle2d::Triangle2d(QPointF a, QPointF b, QPointF c) : a(a), b(b), c(c) {}

QPointF Triangle2d::point(const int n) {
    return n == 0 ? this->a : n == 1 ? this->b : this->c;
}

Triangle2d Triangle2d::operator +(const QPointF& v) {
    return Triangle2d(QPointF(a + v), QPointF(b + v), QPointF(c + v));
}

Triangle2d& Triangle2d::operator +=(const QPointF& v) {
    *this = Triangle2d(QPointF(a + v), QPointF(b + v), QPointF(c + v));
    return *this;
}

Triangle2d Triangle2d::operator -(const QPointF& v) {
    return Triangle2d(QPointF(a - v), QPointF(b - v), QPointF(c - v));
}

Triangle2d& Triangle2d::operator -=(const QPointF& v) {
    *this = Triangle2d(QPointF(a - v), QPointF(b - v), QPointF(c - v));
    return *this;
}

Triangle2d Triangle2d::operator *(const qreal &v) {
    return Triangle2d(QPointF(a * v), QPointF(b * v), QPointF(c * v));
}

Triangle2d Triangle2d::operator /(const qreal &v) {
    return Triangle2d(QPointF(a / v), QPointF(b / v), QPointF(c / v));
}

Triangle2d& Triangle2d::rotate(const QPointF& C, const qreal angle) {
    *this = Triangle2d(rotatePt(a, C, angle), rotatePt(b, C, angle), rotatePt(c, C, angle));
    return *this;
}

bool Triangle2d::overlap(const Triangle2d& t) {
    return li(this->a, this->b, t.a, t.b) || li(this->a, this->b, t.b, t.c) || li(this->a, this->b, t.c, t.a)
    || li(this->b, this->c, t.a, t.b) || li(this->b, this->c, t.b, t.c) || li(this->b, this->c, t.c, t.a)
        || li(this->c, this->a, t.a, t.b) || li(this->c, this->a, t.b, t.c) || li(this->c, this->a, t.c, t.a);
}

QPointF Triangle2d::centroid() {
    return QPointF(
        (a.x() + b.x() + c.x()) / 3,
        (a.y() + b.y() + c.y()) / 3
        );
}
