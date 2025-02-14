#include "depliagevue2d.h"
#include <qgraphicsitem.h>

DepliageVue2d::DepliageVue2d () {}

DepliageVue2d::DepliageVue2d (QWidget *parent) : QGraphicsView(parent) {
    setFrameStyle(QFrame::Panel);
    it = nullptr;
    git = nullptr;
}

void DepliageVue2d::mousePressEvent (QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        git = itemAt(event->position().x(), event->position().y());
        if (git) {
            while(git->parentItem()) {
                git = git->parentItem();
            }
            event->accept();
            //return;
        }
    } else
    if (event->button() == Qt::MiddleButton) {
        _pan = true;
        _panStartX = event->position().x();
        _panStartY = event->position().y();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        it = itemAt(event->position().x(), event->position().y());
        while(it->parentItem()) {
            it = it->parentItem();
        }

        return;
    }
    QGraphicsView::mousePressEvent (event);
}

void DepliageVue2d::mouseReleaseEvent (QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        _pan = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        it = nullptr;
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
                //qreal dX = dy/18;
                if (it) {
                    emit tourne2D (dx, it);
                    //emit tourne2D (dX);
                    _panStartX = event->position().x();
                    _panStartY = event->position().y();
                    event->accept();
                    return;
                }
            }
        }
    }
    QGraphicsView::mouseMoveEvent (event);
}

/*void DepliageVue2d::wheelEvent (QWheelEvent *event) {
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
}*/
