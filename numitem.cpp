#include "numitem.h"


NumItem::NumItem() {}
NumItem::NumItem(TriangleLigneItem *li, TriangleLangItem *lai, int num, int dYt, bool bLang) {
    this->dYt = dYt;
    this->tli = li;
    this->bLang = bLang;
    this->lai = lai;
    setZValue(4);
    setParentItem(li);
    setBrush(QBrush(Qt::blue));
    setText(QString::number(num));
    QFont tf = QFont("Bitstream Vera Sans", 7);
    tf.setLetterSpacing(QFont::AbsoluteSpacing, -2);
    setFont(tf);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    setData(0, QVariant::fromValue(-1));    

    tourne();
}

void NumItem::tourne() {
    QPointF b;
    QPointF centre;

    if (bLang) {
        centre  = centroid4(lineToLang(tli->line().p1(), tli->line().p2()).toFillPolygon());
        b = QPointF(centre.x() -boundingRect().width()/2,
                    centre.y() -boundingRect().height()/2 + 1);
        setTransformOriginPoint(boundingRect().center());
        setRotation(180- tli->line().angle());
        setPos(b);
    } else {
        b = QPointF(boundingRect().width()/2,
                    boundingRect().height()+ dYt);
        setTransformOriginPoint(b);
        setRotation(180- tli->line().angle());
        QPointF centre = tli->line().center() - b;
        setPos(centre);
    }
}

void NumItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug() << tli->ligne->id1 << tli->ligne->id2;
    QGraphicsSimpleTextItem::mousePressEvent(event);
}
