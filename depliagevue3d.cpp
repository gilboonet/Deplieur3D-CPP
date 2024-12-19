// QGraphicsView de la vue 3D avec zoom et rotation caméra
//---------------------------------------------------------
#include "depliagevue3d.h"
//---------------------------------------------------------
DepliageVue3d::DepliageVue3d () {}

DepliageVue3d::DepliageVue3d (QWidget *parent) : QGraphicsView(parent) {}

void DepliageVue3d::mousePressEvent (QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        _pan = true;
        _panStartX = event->position().x();
        _panStartY = event->position().y();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QGraphicsView::mousePressEvent (event);
}

void DepliageVue3d::mouseReleaseEvent (QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        _pan = false;
        setCursor (Qt::ArrowCursor);
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent (event);
}

void DepliageVue3d::mouseMoveEvent (QMouseEvent *event) {
    if (_pan) {
        qreal dx = event->position().x() - _panStartX;
        qreal dy = event->position().y() - _panStartY;
        qreal adx = qFabs(dx);
        qreal ady = qFabs(dy);
        if (adx > 4 || ady > 3) {
            if (adx < 20 && ady < 15) {
                qreal dZ = dx/24;
                qreal dX = dy/18;
                emit tourneModele (dZ, dX);
                _panStartX = event->position().x();
                _panStartY = event->position().y();
                event->accept();
                return;
            }
        }
    }
    QGraphicsView::mouseMoveEvent (event);
}

void DepliageVue3d::wheelEvent (QWheelEvent *event) {
    if (event->buttons().testFlag(Qt::RightButton)) {
        int d = event->angleDelta().y();
        if (d < 0)
            this->scale(0.9f, 0.9f);
        else if (d>0)
            this->scale(1.1f, 1.1f);
        return;
    } else
        event->ignore();

    QGraphicsView::wheelEvent (event);
}
