// QGraphicsLineItem pour ligne de coupe ou pli
//---------------------------------------------------------
#include "pieceligneitem.h"
#include "piecelangitem.h"
#include "piecenumitem.h"
//---------------------------------------------------------
PieceLigneItem::PieceLigneItem () {}
PieceLigneItem::PieceLigneItem (PiecePolygonItem *parent, Ligne *ligne, int num) {
    QPen pliMontagne (QBrush(Qt::darkGreen), 1, Qt::DashLine);
    QPen pliVallee   (QBrush(Qt::darkRed), 1, Qt::DashDotDotLine);
    QPen pliAucun    (QBrush(Qt::NoBrush), 1, Qt::NoPen);
    QPen pliBord     (QBrush(Qt::red), 1, Qt::SolidLine);

    setParentItem(parent);
    setLine(ligne->p1.x(), ligne->p1.y(),
            ligne->p2.x(), ligne->p2.y());
    this->ligne = ligne;

    setFlag(QGraphicsItem::ItemIsSelectable);
    setData(0, QVariant(ligne->id1));
    setData(1, QVariant(ligne->id2));
    setCursor(QCursor(Qt::PointingHandCursor));
    moveBy(-parentItem()->x(),  - parentItem()->y());

    PieceLangItem *pli = new PieceLangItem(ligne);
    pli->setParentItem(this);

    if (ligne->nb == 2)
        setPen((ligne->cop < 0) ? pliVallee
            :  (ligne->cop > 0) ? pliMontagne
                                : pliAucun   );
    else {
        if (ligne->bLang)
            setPen((ligne->cop < 0) ? pliVallee
                   :  (ligne->cop > 0) ? pliMontagne
                                      : pliAucun   );
        else
            setPen(pliBord);
        PieceNumItem *gti = new PieceNumItem(this, ligne, num);
    }
    setZValue(2);
    QPointF deltaPiece = parentItem()->pos();
    moveBy(deltaPiece.x(), deltaPiece.y());
}

void PieceLigneItem::mousePressEvent (QGraphicsSceneMouseEvent *event) {
    qDebug() << this->ligne->id1 << this->ligne->id2;
}
//---------------------------------------------------------
QPointF centroid4 (QPolygonF p) {
    QPointF p1 = (p[0] + p[1]) / 2;
    QPointF p2 = (p[2] + p[3]) / 2;

    QPointF r = (p1 + p2) /2;

    return r;
}
