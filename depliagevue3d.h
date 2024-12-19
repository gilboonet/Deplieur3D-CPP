// QGraphicsView de la vue 3D avec zoom et rotation caméra
//---------------------------------------------------------
#ifndef DEPLIAGEVUE3D_H
#define DEPLIAGEVUE3D_H

#include <QGraphicsView>
#include <QMouseEvent>
//---------------------------------------------------------
class DepliageVue3d : public QGraphicsView
{
    Q_OBJECT
public:
    DepliageVue3d ();
    DepliageVue3d (QWidget*);

signals:
    void tourneModele (qreal, qreal);

private:
    void mouseMoveEvent (QMouseEvent *event) override;
    void mousePressEvent (QMouseEvent *event) override;
    void mouseReleaseEvent (QMouseEvent *event) override;
    void wheelEvent (QWheelEvent *) override;

    bool _pan;
    int _panStartX, _panStartY;
};

#endif // DEPLIAGEVUE3D_H
