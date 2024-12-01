#ifndef TRIANGLELIGNEITEM_H
#define TRIANGLELIGNEITEM_H

#include <QGraphicsLineItem>
#include "structures.h"
#include <QPainter>


class TriangleLigneItem : public QGraphicsLineItem
{
public:
    enum { Type = UserType + 2};
    int type() const override {return(Type);}
    TriangleLigneItem(Ligne *);
    Ligne * ligne;
    QGraphicsLineItem *hoverL;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
};

#endif // TRIANGLELIGNEITEM_H
