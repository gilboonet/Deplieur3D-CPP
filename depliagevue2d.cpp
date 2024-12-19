#include "depliagevue2d.h"

DepliageVue2d::DepliageVue2d () {}

DepliageVue2d::DepliageVue2d (QWidget *parent) : QGraphicsView(parent) {}

void DepliageVue2d::mousePressEvent (QMouseEvent *event) {
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

void DepliageVue2d::mouseReleaseEvent (QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        _pan = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent (event);
}

void DepliageVue2d::mouseMoveEvent (QMouseEvent *event) {
    if (_pan) {
        qreal dx = event->position().x() - _panStartX;
        qreal dy = event->position().y() - _panStartY;
        qreal adx = qFabs(dx);
        qreal ady = qFabs(dy);
        if (adx > 4 || ady > 3) {
            if (adx < 20 && ady < 15) {
                //qreal dZ = dx;
                qreal dX = dy/18;
                emit tourne2D (dx);
                _panStartX = event->position().x();
                _panStartY = event->position().y();
                event->accept();
                return;
            }
        }
    }
    QGraphicsView::mouseMoveEvent (event);
}

void DepliageVue2d::wheelEvent (QWheelEvent *event) {
    if (event->buttons().testFlag(Qt::RightButton)) {
        int d = event->angleDelta().y();
        if (d < 0)
            this->scale(0.9f, 0.9f);
        else if (d>0)
            this->scale(1.1f, 1.1f);
        return;
    } else
        event->ignore();

    QGraphicsView::wheelEvent(event);
}
