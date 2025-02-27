// QGraphicsSimpleTextItem pour numéro d'arête
//---------------------------------------------------------
#ifndef PIECENUMITEM_H
#define PIECENUMITEM_H

#include <QGraphicsSimpleTextItem>

#include "facette.h"
#include "pieceligneitem.h"
#include "depliagescene.h"
//---------------------------------------------------------
class PieceNumItem : public QGraphicsSimpleTextItem
{
public:
    enum { Type = UserType + 6};
    int type() const override {return(Type);}
    Ligne *ligne;
    DepliageScene *sceneD;

    PieceNumItem ();
    PieceNumItem (PieceLigneItem *, Ligne*, int, bool = false);

    void mousePressEvent(QGraphicsSceneMouseEvent *) override;
    void hoverEnterEvent (QGraphicsSceneHoverEvent *) override;
    void hoverLeaveEvent (QGraphicsSceneHoverEvent *) override;

    void basculeMode (bool);
};

#endif // PIECENUMITEM_H
