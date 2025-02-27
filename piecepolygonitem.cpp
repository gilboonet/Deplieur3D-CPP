// QGraphicsPolygonItem pour bord de pièce
//---------------------------------------------------------
#include "piecepolygonitem.h"
//---------------------------------------------------------
PiecePolygonItem::PiecePolygonItem () {}
PiecePolygonItem::PiecePolygonItem (DepliageScene *scene, QColor couleur, int n) {
    sceneD = scene;
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);
    setPen(Qt::NoPen);
    setBrush(QBrush(couleur));
    setZValue(2);
    setCursor(Qt::OpenHandCursor);
    scene->addItem(this);
    nPiece = n;
    hPt = QPoint();
}

void PiecePolygonItem::paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QStyleOptionGraphicsItem opt = *option;

    if (isSelected()) {
        opt.state = QStyle::State_None;
        setPen(QPen(Qt::black, 2));
    } else {
        setPen(Qt::NoPen);
    }

    QGraphicsPolygonItem::paint (painter, &opt, widget);
}

void PiecePolygonItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    QPoint pt = event->pos().toPoint();
    if (!(hPt == pt)) {
        hPt = event->pos().toPoint();
        //qreal dx = hPt.x();
        //qreal dy = hPt.y();

        emit sceneD->pieceHoverOn(nPiece);
        //qDebug() << "Piece #" << this->nPiece << dx << dy;
    }
}

void PiecePolygonItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    emit sceneD->pieceHoverOff(nPiece);
}
