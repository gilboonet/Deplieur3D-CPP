// QGraphicsPolygonItem pour item Triangle de la vue 3D
//---------------------------------------------------------
#include "triangleitem3d.h"

#include <QPen>
//---------------------------------------------------------
TriangleItem3d::TriangleItem3d () {
    pieceCouleur = QColor();
    id = -1;
    col = -1;
    sceneD = nullptr;
}

TriangleItem3d::TriangleItem3d (DepliageScene *scene, QColor pieceCouleur, QPolygonF poly, int id, int col = 0) {
    this->pieceCouleur = pieceCouleur;
    this->sceneD = scene;
    this->id = id;
    setData(0, QVariant(id));
    this->col = col;
    setPolygon(poly);
    setFlag(ItemIsSelectable);
    setCursor(Qt::PointingHandCursor);
    setAcceptHoverEvents(true);
    scene->addItem(this);
}

void TriangleItem3d::paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QStyleOptionGraphicsItem opt = *option;
    if (isSelected())
        opt.state = QStyle::State_None;

    setPen(QPen());
    setBrush(QBrush(this->pieceCouleur, this->hoverOn ? Qt::Dense3Pattern: Qt::SolidPattern));

    QGraphicsPolygonItem::paint(painter, &opt, widget);
}

void TriangleItem3d::editeTriangle () {
    bool ok = false;
    if (sceneD->itemColorId >= 0) {
        if (col != sceneD->itemColorId) {
            emit sceneD->peutColorierFace(this->id);
        } else {
            emit sceneD->pieceEnleveFace(this->col, this->id);
        }
        sceneD->update(boundingRect());
    }
}

void TriangleItem3d::mousePressEvent (QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        editeTriangle();
    } else if (event->button() == Qt::RightButton) {
        emit sceneD->changeCouleur(col);
    }
    //QGraphicsPolygonItem::mousePressEvent(event);
}

void TriangleItem3d::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    this->hoverOn = true;
    if (this->col > 0)
        emit sceneD->hoverOn(this->id);
    update();
}

void TriangleItem3d::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    this->hoverOn = false;
    emit sceneD->hoverOff(this->id);
    update();
}
