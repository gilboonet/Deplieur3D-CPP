#include "piecenumitem.h"

PieceNumItem::PieceNumItem() {}

PieceNumItem::PieceNumItem (PieceLigneItem *parentItem, Ligne *ligne, int num, bool estEditable) {
    this->ligne = ligne;
    setParentItem(parentItem);
    sceneD = static_cast<DepliageScene*>(this->scene());
    setZValue(5);
    setBrush(QBrush(Qt::blue));
    setText(QString::number(num));
    QFont tf = QFont();
    tf.setPointSize(7); //"Bitstream Vera Sans", 7);
    tf.setLetterSpacing(QFont::AbsoluteSpacing, -2);
    setFont(tf);
    basculeMode(estEditable);

    QPointF b = QPointF(boundingRect().width() / 2,
                        boundingRect().height());
    setTransformOriginPoint(b);
    setRotation(180- parentItem->line().angle());
    QPointF centre = parentItem->line().center() - b;
    setPos(centre);
}

void PieceNumItem::basculeMode(bool estEditable) {
    setFlag(QGraphicsItem::ItemIsSelectable, estEditable);
    setAcceptHoverEvents(estEditable);
    setCursor(estEditable ? Qt::PointingHandCursor : Qt::ArrowCursor);

}

void PieceNumItem::mousePressEvent (QGraphicsSceneMouseEvent *event)
{
    emit sceneD->basculeLanguette(ligne->id1, ligne->id2);
}

void PieceNumItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setBrush(QBrush(Qt::yellow));
}

void PieceNumItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setBrush(QBrush(Qt::blue));
}
