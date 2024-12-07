#include "triangleligneitem.h"

TriangleLigneItem::TriangleLigneItem(Ligne *ligne) {
    this->ligne = ligne;
    setLine(
        ligne->p1.x(), ligne->p1.y(),
        ligne->p2.x(), ligne->p2.y());
    setZValue(3);
    setData(0, QVariant::fromValue(-1));
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    //setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setAcceptHoverEvents(true);
}

void TriangleLigneItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen p = QPen();

    bool lb = data(1).toBool();

    if (ligne->nb == 1 && !lb) {
        p.setColor(Qt::black);
        p.setStyle(Qt::SolidLine);
    } else {
        p.setColor(ligne->cop > 0 ? Qt::darkRed : ligne->cop < 0 ? Qt::green : Qt::black);
        p.setStyle(ligne->cop > 0 ? Qt::DashLine : ligne->cop < 0 ? Qt::DashDotDotLine : Qt::NoPen);
    }

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
