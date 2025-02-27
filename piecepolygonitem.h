// QGraphicsPolygonItem pour bord de pièce
//---------------------------------------------------------
#ifndef PIECEPOLYGONITEM_H
#define PIECEPOLYGONITEM_H

#include <QGraphicsPolygonItem>
#include <QGraphicsScene>
#include <QColor>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneEvent>

#include "depliagescene.h"
//---------------------------------------------------------
class PiecePolygonItem : public QGraphicsPolygonItem
{
public:
    enum { Type = UserType + 3};
    int type() const override {return(Type);}
    int nPiece;
    //QPointF hPoint;
    QPoint hPt;
    DepliageScene *sceneD;

    PiecePolygonItem ();
    PiecePolygonItem (DepliageScene *, QColor, int);

    void paint (QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override;
    void hoverMoveEvent (QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent (QGraphicsSceneHoverEvent *event) override;
};

#endif // PIECEPOLYGONITEM_H
