// QGraphicsPolygonItem pour bord de pièce
//---------------------------------------------------------
#ifndef PIECEPOLYGONITEM_H
#define PIECEPOLYGONITEM_H

#include <QGraphicsPolygonItem>
#include <QGraphicsScene>
#include <QColor>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
//---------------------------------------------------------
class PiecePolygonItem : public QGraphicsPolygonItem
{
public:
    enum { Type = UserType + 3};
    int type() const override {return(Type);}

    PiecePolygonItem ();
    PiecePolygonItem (QGraphicsScene *, QColor);

    void paint (QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override;

};

#endif // PIECEPOLYGONITEM_H
