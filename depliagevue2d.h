// QGraphicsView du gabarit 2D avec zoom et rotation objet courant
#ifndef DEPLIAGEVUE2D_H
#define DEPLIAGEVUE2D_H

#include <QGraphicsView>
#include <QMouseEvent>

class DepliageVue2d : public QGraphicsView
{
    Q_OBJECT
public:
    DepliageVue2d ();
    DepliageVue2d (QWidget *);
    QGraphicsItem *git;

signals:
    void tourne2D (qreal, QGraphicsItem*);

private:
    void mouseMoveEvent (QMouseEvent *event) override;
    void mousePressEvent (QMouseEvent *event) override;
    void mouseReleaseEvent (QMouseEvent *event) override;
    //void wheelEvent (QWheelEvent *) override;

    bool _pan;
    int _panStartX, _panStartY;
    QGraphicsItem *it;
};

#endif // DEPLIAGEVUE2D_H
