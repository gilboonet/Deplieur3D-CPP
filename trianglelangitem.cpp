#include "trianglelangitem.h"

TriangleLangItem::TriangleLangItem() {}

QPainterPath lineToLang(QPointF p1, QPointF p2) {
    QPainterPath lang;
    QLineF ligne(p1, p2);
    qreal l = ligne.length();
    if (l -10 > 10) {
        lang.lineTo(QPointF(10,10));
        lang.lineTo(QPointF(10, l-10));
    } else {
        lang.lineTo(QPointF(10, l-10));
        lang.lineTo(QPointF(10,10));
    }
    lang.lineTo(QPointF(0, l));
    QTransform tr;
    tr.translate(p1.x(), p1.y()).rotate(270 -ligne.angle());
    return tr.map(lang);
}

TriangleLangItem::TriangleLangItem(Ligne *ligne)
{
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    setPath(lineToLang(ligne->p1, ligne->p2));
}

void TriangleLangItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    bool b = data(1).toBool();
    if (!b)
        return;

    QPen p = QPen();
    p.setColor(Qt::black);
    p.setStyle(Qt::SolidLine);

    painter->setPen(QPen( QBrush(Qt::NoBrush), 0));
    painter->setBrush(QBrush(QColor::fromString("ghostwhite"), Qt::SolidPattern));
    QPolygonF poly = path().toFillPolygon();
    painter->drawPolygon(poly);
    painter->setPen(p);
    painter->drawPath(path());
}

void TriangleLangItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "clic";
}
