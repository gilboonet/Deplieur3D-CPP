// QGraphicsScene avec signaux lancés par ses items
//---------------------------------------------------------
#ifndef DEPLIAGESCENE_H
#define DEPLIAGESCENE_H

#include <QGraphicsScene>
#include <QObject>
#include <QGraphicsRectItem>
#include <QPainter>


class DepliageScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit DepliageScene (QObject *parent = nullptr, bool doBackground = true, int margeId = 0);
    QColor itemColor;
    int itemColorId = -1;
    int nbPages;
    bool doBackground;
    int margeId; // 0 : sans - 1 : Cricut - 2 : Brother - 3 : Silhouette
    int typeLang; // 0, 1 ou 2
    QGraphicsRectItem *pageTemoin = nullptr;
    QGraphicsLineItem *ligneTemoin = nullptr;
    int faceCourante = -1;
    int dernFace = -1;
    QPoint dim = QPoint(210, 297);

signals:
    void changeCouleur (int);
    void changeNBCouleur (int, int);
    void changeFaceCouleur (int, int);
    void peutColorierFace (int, int = -1);
    void pieceEnleveFace (int);
    void pieceEnleveFaces (int, int);
    void basculeLanguette(int, int);
    void hoverOn(int);
    void ligneHoverOn(int, int);
    void hoverOff(int);

protected:
    void drawBackground (QPainter *, const QRectF &) override;
};

#endif // DEPLIAGESCENE_H
