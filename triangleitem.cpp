#include "triangleitem.h"

#include <QTableWidget>
#include <QPainter>
#include "deplieurscene.h"
//#include <QGraphicsItem>

TriangleItem::TriangleItem(QColor poolColor, QPolygonF poly, int id, int col = 0)
{
    this->poolColor = poolColor;
    this->setParentItem(0);
    this->id = id;
    setData(0, QVariant(id));
    this->col = col;
    setPolygon(poly);
    setFlag(ItemIsSelectable);
    setFlag(ItemIsFocusable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemSendsScenePositionChanges);
    estLie = false;
    estPrem = false;
    //setVisible(false);
}

void TriangleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen p;
    QStyleOptionGraphicsItem opt = *option;
    if (isSelected()) {
        p = QPen(QPen(Qt::yellow));
        setBrush(QBrush(Qt::red));
        opt.state = QStyle::State_None;
    } else {
        QColor c = this->poolColor;
        // if (estPrem)
        //     c = c.lighter();
        c.setAlpha(235);
        if (estLie || estPrem) {
            p = QPen(c);
        } else {
            p = QPen(Qt::black);
        }
        setPen(p);
        setBrush(QBrush(c, Qt::SolidPattern));
    }
    QGraphicsPolygonItem::paint(painter, &opt, widget);
}

void TriangleItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *)
{
    DeplieurScene *sc = dynamic_cast<DeplieurScene*>(this->scene());

    if (!sc)
        return;
    emit sc->changeCouleur(this->col);
}

void TriangleItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Qt::KeyboardModifiers keys;
    keys.setFlag(Qt::ControlModifier);
    event->setModifiers(keys);
    if (estLie)
        qgraphicsitem_cast<TriangleItem*>(parentItem())->mouseMoveEvent(event);
    else
        QGraphicsItem::mouseMoveEvent(event);
}

void TriangleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Qt::KeyboardModifiers keys;
    keys.setFlag(Qt::ControlModifier);
    event->setModifiers(keys);
    if (estLie)
        qgraphicsitem_cast<TriangleItem*>(parentItem())->mouseReleaseEvent(event);
    else
        QGraphicsItem::mouseReleaseEvent(event);
}

void TriangleItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Qt::KeyboardModifiers keys;
    keys.setFlag(Qt::ControlModifier);
    event->setModifiers(keys);
    QGraphicsPolygonItem::mousePressEvent(event);
}
