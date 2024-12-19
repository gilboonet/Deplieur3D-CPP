// QGraphicsPathItem pour languette
//---------------------------------------------------------
#ifndef PIECELANGITEM_H
#define PIECELANGITEM_H

#include <QGraphicsPathItem>

#include "facette.h"
//---------------------------------------------------------
class PieceLangItem : public QGraphicsPathItem
{
public:
    enum { Type = UserType + 5};
    int type() const override {return(Type);}
    Ligne *ligne;

    PieceLangItem ();
    PieceLangItem (Ligne *ligne);

    virtual void paint (QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override;
};

QPainterPath lineToLang (QPointF, QPointF, bool = false);

#endif // PIECELANGITEM_H
