#include "triangleitem2d.h"

#include <QPen>
//---------------------------------------------------------
TriangleItem2d::TriangleItem2d() {
    pieceCouleur = QColor();
    id = -1;
    col = -1;
    sceneD = nullptr;
}

TriangleItem2d::TriangleItem2d(DepliageScene *scene, QColor pieceCouleur, QPolygonF poly, int id, int col = 0)
{
    this->pieceCouleur = pieceCouleur;
    this->sceneD = scene;
    this->id = id;
    setData(0, QVariant(id));
    this->col = col;
    setPolygon(poly);
    this->setVisible(false);
    scene->addItem(this);
}

void TriangleItem2d::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QStyleOptionGraphicsItem opt = *option;
    if (isSelected())
        opt.state = QStyle::State_None;

    setPen(QPen(Qt::NoPen));
    setBrush(QBrush(hoverOn ? this->pieceCouleur.darker() : this->pieceCouleur, Qt::SolidPattern));

    QGraphicsPolygonItem::paint(painter, &opt, widget);
}
