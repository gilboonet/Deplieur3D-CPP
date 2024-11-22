#ifndef TRIANGLEITEM_H
#define TRIANGLEITEM_H

#include "mainwindow.h"
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QPen>
#include <QBrush>
#include <QStyleOptionGraphicsItem>

class MainWindow;

class TriangleItem : public QGraphicsPolygonItem
{
public:
    TriangleItem();
    TriangleItem(MainWindow *, QPolygonF, int, int);
    MainWindow *w;
    int id;
    int col;
    bool estPrem = false;
    bool estLie = false;
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
    enum { Type = UserType + 1 };
    int type();

};

#endif // TRIANGLEITEM_H
