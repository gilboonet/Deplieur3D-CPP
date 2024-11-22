#ifndef DEPLIAGE_H
#define DEPLIAGE_H

#include "triangleitem.h"
#include "mesh.h"
#include "mat4x4.h"

#include <QGraphicsScene>
#include <QPointer>


class MainWindow;
class TriangleItem;

struct Attache {
    int de;     // -1 si première face
    int vers;
};

struct Pool {
    QColor couleur;
    QList<int> elements;
    QList<std::vector<Attache>> pieces;
};

class Depliage
{
public:
    Depliage();
    Depliage(MainWindow*);

    QPointer<MainWindow> parent;
    QGraphicsScene *scene3d = nullptr;
    QGraphicsScene *scene2d = nullptr;
    std::vector<QGraphicsRectItem *> pages;
    std::vector<TriangleItem *> t2d;
    Mesh *meshModel = nullptr;
    std::vector<int> sauveSel;
    QVector<Pool> pool;

    float fYaw;
    static constexpr float fPas = 0.15f;
    float fThetaX;
    float fThetaY;
    float fThetaZ;

    void dessineModele();
    void creeFaces2d();
    void trouveVoisinage();
    void basculeSelectionChanged(bool);
};

int next(int);
int prev(int);

#endif // DEPLIAGE_H
