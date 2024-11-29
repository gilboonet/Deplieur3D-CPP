#ifndef TRIANGLEITEM_H
#define TRIANGLEITEM_H

//#include "mainwindow.h"
#include "structures.h"

#include <QList>
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QPen>
#include <QBrush>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneEvent>

//class MainWindow;
//class Depliage;

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
    //void mousePressEvent(QGraphicsSceneMouseEvent *);
    //void mouseMoveEvent(QGraphicsSceneMouseEvent *);
    //void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
    //QVariant itemChange(GraphicsItemChange change, const QVariant &value);
};

#endif // TRIANGLEITEM_H
