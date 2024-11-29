#include "triangleitem.h"
//#include "depliage.h"

#include <QTableWidget>
#include <QPainter>
#include "deplieurscene.h"

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
    //setCacheMode(NoCache);
    //setZValue(-1);
    estLie = false;
    estPrem = false;
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
        if (estPrem)
            c = c.darker();
        //c.setAlpha(estPrem ? 200 : 235);
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

    sc->twC->selectRow(this->col);
}

/*QVariant TriangleItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionHasChanged && scene()) {
        QPointF delta = value.toPointF() - this->pos();
        QPolygonF tp = this->polygon();
        QTransform tr;
        tr.translate(delta.x(), delta.y());
        this->setPolygon(tr.map(tp));
    }

    return value;
    //return QGraphicsItem::itemChange(change, value);
}*/
/*
void TriangleItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (estLie) {
        event->ignore();
    }
    QGraphicsPolygonItem::mousePressEvent(event);
}

void TriangleItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (estLie) {
        event->ignore();
    }
    QGraphicsPolygonItem::mouseMoveEvent(event);
}

void TriangleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (estLie) {
        event->ignore();
    }
    QGraphicsPolygonItem::mouseReleaseEvent(event);
}
*/
