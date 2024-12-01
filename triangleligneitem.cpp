#include "triangleligneitem.h"

TriangleLigneItem::TriangleLigneItem(Ligne *ligne) {
    this->ligne = ligne;
    setLine(
        ligne->p1.x(), ligne->p1.y(),
        ligne->p2.x(), ligne->p2.y());
    setZValue(3);
    setData(0, QVariant::fromValue(-1));
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setAcceptHoverEvents(true);
}

void TriangleLigneItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen p = QPen();
    p.setColor(ligne->nb == 1 ? Qt::black : ligne->cop < 1 ? Qt::darkRed : Qt::green);
    if (ligne->nb == 1)
        p.setStyle(Qt::SolidLine);
    else if (ligne->cop == 0)
        p.setStyle(Qt::NoPen);
    else
        p.setStyle(ligne->cop < 1 ? Qt::DashLine : Qt::DashDotLine);

    painter->setPen(p);
    painter->drawLine(line());
}

void TriangleLigneItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    hoverL = new QGraphicsLineItem();

    hoverL->setLine(line());
    hoverL->setZValue(20);
    hoverL->setPen(QPen(QBrush(Qt::yellow, Qt::Dense5Pattern), 2));

    hoverL->setParentItem(parentItem());
}

void TriangleLigneItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    delete(hoverL);
}
