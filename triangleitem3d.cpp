// QGraphicsPolygonItem pour item Triangle de la vue 3D
//---------------------------------------------------------
#include "triangleitem3d.h"
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
    this->hoverOn = false;
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

    QPen p = QPen();
    p.setWidth(1);
    setPen(p);
    setBrush(QBrush(this->hoverOn ? this->pieceCouleur.darker() : this->pieceCouleur, Qt::SolidPattern));

    QGraphicsPolygonItem::paint(painter, &opt, widget);
}

void TriangleItem3d::editeTriangle () {
    bool ok = false;
    if (sceneD->itemColorId >= 0) {
        if (col != sceneD->itemColorId) {
            emit sceneD->peutColorierFace(this->id);
        } else {
            emit sceneD->pieceEnleveFace(this->id);
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

void TriangleItem3d::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    if (sceneD->gereHover) {
        this->hoverOn = true;
        if (sceneD->faceCourante != this->id) {
            sceneD->dernFace = sceneD->faceCourante;
            sceneD->faceCourante = this->id;
        }
        if (sceneD->triangleHover) {
            TriangleItem3d *t = qgraphicsitem_cast<TriangleItem3d*>(sceneD->triangleHover);
            if (t) {
                t->hoverOn = false;
                t->update();
            }
        }
        sceneD->triangleHover = this;
        if (this->col > 0)
            emit sceneD->facetteHoverOn(this->id);
        update();
    }
}

void TriangleItem3d::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    if (sceneD->gereHover) {
        this->hoverOn = false;
        update();
        sceneD->triangleHover = nullptr;
        emit sceneD->facetteHoverOff(this->id);
    }
}
