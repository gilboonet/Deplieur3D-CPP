// QGraphicsLineItem pour ligne de coupe ou pli
//---------------------------------------------------------
#ifndef PIECELIGNEITEM_H
#define PIECELIGNEITEM_H

#include <QGraphicsLineItem>

#include "facette.h"
#include "piecepolygonitem.h"
//---------------------------------------------------------
class PieceLigneItem : public QGraphicsLineItem
{
public:
    enum { Type = UserType + 4};
    int type() const override {return(Type);}

    PieceLigneItem ();
    PieceLigneItem (PiecePolygonItem *, Ligne*, int);

    Ligne *ligne;

    void mousePressEvent (QGraphicsSceneMouseEvent *event) override;
};
//---------------------------------------------------------
QPointF centroid4 (QPolygonF);

#endif // PIECELIGNEITEM_H
