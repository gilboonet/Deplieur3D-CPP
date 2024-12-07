#ifndef TRIANGLELANGITEM_H
#define TRIANGLELANGITEM_H

#include <QGraphicsPathItem>
#include "structures.h"
#include <QPainter>


class TriangleLangItem : public QGraphicsPathItem
{
public:
    enum { Type = UserType + 3};
    int type() const override {return(Type);}
    TriangleLangItem();
    TriangleLangItem(Ligne *);

    virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
};

QPainterPath lineToLang(QPointF, QPointF);

#endif // TRIANGLELANGITEM_H
