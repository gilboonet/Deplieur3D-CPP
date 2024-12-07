#ifndef NUMITEM_H
#define NUMITEM_H

#include <QGraphicsSimpleTextItem>
#include "structures.h"
#include <QPainter>

#include "triangleligneitem.h"
#include "trianglelangitem.h"


class NumItem : public QGraphicsSimpleTextItem
{
public:
    enum { Type = UserType + 4};
    int type() const override {return(Type);}
    int dYt;
    bool bLang;
    TriangleLigneItem *tli;
    TriangleLangItem *lai;

    NumItem();
    NumItem(TriangleLigneItem *, TriangleLangItem *, int, int, bool);

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void tourne();
};

#endif // NUMITEM_H
