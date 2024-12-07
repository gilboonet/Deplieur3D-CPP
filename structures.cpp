#include "structures.h"

Nums::Nums() {}
Nums::Nums(int n1, int n2) : id1(n1), id2(n2) {}
Nums::Nums(int n1, int n2, int num) : id1(n1), id2(n2), num(num) {}
bool Nums::operator==(const Nums &n) const
{
    return
        (this->id1 == n.id1 && this->id2 == n.id2)
     || (this->id1 == n.id2 && this->id2 == n.id1);
}

QPointF centroid(QPolygonF poly) {
    QPointF pt = QPoint();
    for (auto&& pts : poly)
        pt += pts;
    pt /= poly.size();

    return pt;
}

QPointF centroid4(QPolygonF p) {
    QPointF p1 = (p[0] + p[1]) / 2;
    QPointF p2 = (p[2] + p[3]) / 2;

    QPointF r = (p1 + p2) /2;

    return r;
}
