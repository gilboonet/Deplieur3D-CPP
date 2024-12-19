// QGraphicsPathItem pour languette
//---------------------------------------------------------
#include "piecelangitem.h"
//---------------------------------------------------------
PieceLangItem::PieceLangItem () {}

PieceLangItem::PieceLangItem (Ligne *ligne) {
    this->ligne = ligne;
    setPath(lineToLang(ligne->p1, ligne->p2));
    setZValue(1);
}

void PieceLangItem::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    if (!ligne->bLang || ligne->nb == 2)
        return;

    painter->setPen(QPen( QBrush(Qt::NoBrush), 0));
    painter->setBrush(QBrush(QColor::fromString("ghostwhite"), Qt::SolidPattern));
    QPolygonF poly = path().toFillPolygon();
    painter->drawPolygon(poly);

    QPen p = QPen();
    p.setColor(Qt::red);
    p.setStyle(Qt::SolidLine);
    painter->setPen(p);
    painter->drawPath(path());
}
//---------------------------------------------------------
QPainterPath lineToLang (QPointF p1, QPointF p2, bool aLEchelle) {
    qreal h = 10;
    if (aLEchelle)
        h = h * 5.0f / 1.76f;
    QPainterPath lang;
    QLineF ligne(p1, p2);
    qreal l = ligne.length();
    lang.lineTo(QPointF(h, l/3));
    lang.lineTo(QPointF(h, l - l/3));
    lang.lineTo(QPointF(0, l));
    QTransform tr;
    tr.translate(p1.x(), p1.y()).rotate(270 -ligne.angle());
    return tr.map(lang);
}
