// QGraphicsPolygonItem pour item Triangle de la vue 3D
//---------------------------------------------------------
#ifndef TriangleItem3D_H
#define TriangleItem3D_H

#include "depliagescene.h"

#include <QGraphicsPolygonItem>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneEvent>
//---------------------------------------------------------
class TriangleItem3d : public QGraphicsPolygonItem
{
public:
    enum { Type = UserType + 1 };
    int type() const override { return(Type);}

    TriangleItem3d ();
    TriangleItem3d (DepliageScene *, QColor, QPolygonF, int, int);

    QColor pieceCouleur;
    int id;
    int col;
    DepliageScene *sceneD;
    bool hoverOn;

    void paint (QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override;
    void mousePressEvent (QGraphicsSceneMouseEvent *) override;
    void hoverEnterEvent (QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent (QGraphicsSceneHoverEvent *event) override;

    void editeTriangle ();
};

#endif // TriangleItem3D_H
