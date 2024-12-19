// QGraphicsPolygonItem pour Item triangle de gabarit 2D
//---------------------------------------------------------
#ifndef TRIANGLEITEM2D_H
#define TRIANGLEITEM2D_H

#include "depliagescene.h"

#include <QGraphicsPolygonItem>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneEvent>
//---------------------------------------------------------
class TriangleItem2d : public QGraphicsPolygonItem
{
public:
    enum { Type = UserType + 2 };
    int type() const override { return(Type);}

    TriangleItem2d();
    TriangleItem2d(DepliageScene *, QColor, QPolygonF, int, int);

    QColor pieceCouleur;
    int id;
    int col;
    DepliageScene *sceneD;
    bool hoverOn = false;

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override;
};

#endif // TRIANGLEITEM2D_H
