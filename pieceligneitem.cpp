// QGraphicsLineItem pour ligne de coupe ou pli
//---------------------------------------------------------
#include "pieceligneitem.h"
#include "piecelangitem.h"
#include "piecenumitem.h"
//---------------------------------------------------------
PieceLigneItem::PieceLigneItem () {}
PieceLigneItem::PieceLigneItem (DepliageScene *scene, PiecePolygonItem *parent, Ligne *ligne, int num) {
    this->sceneD = scene;
    const int EP = 1;
    QPen pliMontagne (QBrush(Qt::SolidPattern), EP, Qt::DashLine);
    QPen pliVallee   (QBrush(Qt::SolidPattern), EP, Qt::DashDotDotLine);
    //QPen pliAucun    (QBrush(Qt::NoBrush), 1, Qt::NoPen);
    QPen pliAucun    (QBrush(Qt::SolidPattern), EP/2, Qt::SolidLine);
    QPen pliBord     (QBrush(Qt::SolidPattern), EP, Qt::SolidLine);

    setParentItem(parent);
    setLine(ligne->p1.x(), ligne->p1.y(),
            ligne->p2.x(), ligne->p2.y());
    this->ligne = ligne;

    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);
    setData(0, QVariant(ligne->id1));
    setData(1, QVariant(ligne->id2));
    setCursor(QCursor(Qt::SplitVCursor));
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
        gti->setParentItem(this);
    }
    setZValue(2);
    QPointF deltaPiece = parentItem()->pos();
    moveBy(deltaPiece.x(), deltaPiece.y());
}

void PieceLigneItem::mousePressEvent (QGraphicsSceneMouseEvent *event) {
    qDebug() << this->ligne->id1 << this->ligne->id2;
    if (this->ligne->nb == 1) {
        emit sceneD->peutColorierFace(this->ligne->id1, this->ligne->id2);
    } else {
        emit sceneD->pieceEnleveFaces(this->ligne->id1, this->ligne->id2);
    }
}

void PieceLigneItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QPen p = this->pen();
    p.setColor(Qt::yellow);
    emit sceneD->ligneHoverOn(this->ligne->id1, this->ligne->id2);
    setPen(p);
}

void PieceLigneItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QPen p = this->pen();
    p.setColor(Qt::black);
    setPen(p);
}
//---------------------------------------------------------
QPointF centroid4 (QPolygonF p) {
    QPointF p1 = (p[0] + p[1]) / 2;
    QPointF p2 = (p[2] + p[3]) / 2;

    QPointF r = (p1 + p2) /2;

    return r;
}
