// QGraphicsScene avec signaux lancés par ses items
//---------------------------------------------------------
#include "depliagescene.h"
//---------------------------------------------------------
DepliageScene::DepliageScene (QObject *parent, bool doBackground, int margeId)
 : QGraphicsScene{parent} {
    this->nbPages = 0;
    this->doBackground = doBackground;
    this->margeId = margeId;
}

void DepliageScene::drawBackground (QPainter *painter, const QRectF &rect) {
    if (!doBackground)
        return;

    QRect page(0, 0, 210, 297);

    QPolygonF pMarge;
    QPointF delta;

    if (margeId == 1) {
        // marge Print then Cut Cricut
        qreal a = 9;
        qreal b = 24;
        qreal c = 118;
        qreal d = 205;

        pMarge << QPoint(a + b, a + b + d + b + a)
           << QPoint(a + b, a + b + d + b)
           << QPoint(a, a + b + d + b)
           << QPoint(a, a + b + d)
           << QPointF(0, a + b + d)
           << QPointF(0, a + b)
           << QPointF(a, a + b)
           << QPointF(a, a)
           << QPointF(a + b, a)
           << QPointF(a + b, 0)
           << QPointF(a + b + c, 0)
           << QPointF(a + b + c, a)
           << QPointF(a + b + c + b, a)
           << QPointF(a + b + c + b, a + b)
           << QPointF(a + b + c + b + a, a + b)
           << QPointF(a + b + c + b + a, a + b + d)
           << QPointF(a + b + c + b, a + b + d)
           << QPointF(a + b + c + b, a + b + d + b)
           << QPointF(a + b + c, a + b + d + b)
           << QPointF(a + b + c, a + b + d + b + a);

        delta = {
                 page.width() - (a+b+c+b+a),
                 page.height() -(a+b+d+b+a) };
        delta /= 2;
    } else if (margeId == 2) {
        // marge Print to Cut Brother

    } else if (margeId == 3) {
        // marge Print & Cut Silhouette
    }

    for (int i = 0; i < this->nbPages; i++) {
        painter->setPen(QPen(QBrush(Qt::blue), 2));
        painter->drawRect(page.translated(i*220, 0));
        if (margeId > 0) {
            painter->setPen(QPen(QBrush(Qt::red), 2));
            painter->drawPolygon(pMarge.translated(delta.x() + 220*i, delta.y()));
        }
    }
}
