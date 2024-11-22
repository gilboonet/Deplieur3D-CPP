#include "triangleitem.h"
#include "depliage.h"

#include <QTableWidget>
#include <QPainter>

TriangleItem::TriangleItem(MainWindow *window, QPolygonF poly, int id, int col = 0)
{
    //w = qobject_cast<MainWindow*>(window);
    w = window;
    this->id = id;
    setData(0, QVariant(id));
    this->col = col;
    setPolygon(poly);
    setPen(QPen(Qt::black));
    QColor c = Qt::white;
    setBrush(QBrush(c, Qt::SolidPattern));
    setFlag(ItemIsSelectable);
    setFlag(ItemIsFocusable);
}

void TriangleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                         QWidget *widget)
{
    QStyleOptionGraphicsItem opt = *option;
    if (isSelected()) {
        setPen(QPen(Qt::yellow));
        setBrush(QBrush(Qt::red));
        opt.state = QStyle::State_None;
    } else {
        setPen(QPen(Qt::black));
        QColor c = this->col < w->dep->pool.size()
                    ? w->dep->pool[this->col].couleur
                    : Qt::white;
        c.setAlpha(235);
        setBrush(QBrush(c, Qt::SolidPattern));
    }
    QGraphicsPolygonItem::paint(painter, &opt, widget);
}

int TriangleItem::type()
{
    return Type;
}
