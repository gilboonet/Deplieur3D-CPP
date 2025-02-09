// QGraphicsPolygonItem pour bord de pièce
//---------------------------------------------------------
#include "piecepolygonitem.h"
//---------------------------------------------------------
PiecePolygonItem::PiecePolygonItem () {}
PiecePolygonItem::PiecePolygonItem (QGraphicsScene *scene, QColor couleur, int n) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setPen(Qt::NoPen);
    setBrush(QBrush(couleur));
    setZValue(1);
    setCursor(Qt::OpenHandCursor);
    scene->addItem(this);
    nPiece = n;
}

void PiecePolygonItem::paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QStyleOptionGraphicsItem opt = *option;
    setPen(Qt::NoPen);
    if (isSelected()) {
        opt.state = QStyle::State_None;
    }

    QGraphicsPolygonItem::paint (painter, &opt, widget);
}
