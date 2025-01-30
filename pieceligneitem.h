// QGraphicsLineItem pour ligne de coupe ou pli
//---------------------------------------------------------
#ifndef PIECELIGNEITEM_H
#define PIECELIGNEITEM_H

#include <QGraphicsLineItem>

#include "facette.h"
#include "piecepolygonitem.h"
#include "depliagescene.h"
//---------------------------------------------------------
class PieceLigneItem : public QGraphicsLineItem
{
public:
    enum { Type = UserType + 4};
    int type() const override {return(Type);}

    PieceLigneItem ();
    PieceLigneItem (DepliageScene *scene, PiecePolygonItem *, Ligne*, int);

    Ligne *ligne;
    DepliageScene *sceneD;

    void mousePressEvent (QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent (QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent (QGraphicsSceneHoverEvent *event) override;
};
//---------------------------------------------------------
QPointF centroid4 (QPolygonF);

#endif // PIECELIGNEITEM_H
