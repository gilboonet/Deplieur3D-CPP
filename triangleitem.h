#ifndef TRIANGLEITEM_H
#define TRIANGLEITEM_H

#include "structures.h"

#include <QList>
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QPen>
#include <QBrush>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneEvent>


class TriangleItem : public QGraphicsPolygonItem
{
public:
    enum { Type = UserType + 1 };
    int type() const override { return(Type);}
    TriangleItem();
    TriangleItem(QColor, QPolygonF, int, int);
    QColor poolColor;
    int id;
    int col;
    bool estPrem = false;
    bool estLie = false;
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // TRIANGLEITEM_H
