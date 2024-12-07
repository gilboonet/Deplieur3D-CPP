#ifndef DEPLIAGE_H
#define DEPLIAGE_H

#include "mainwindow.h"
#include "triangleitem.h"
#include "mesh.h"
#include "mat4x4.h"

#include <QPointer>
#include "deplieurscene.h"
#include <QTableWidget>

#include "structures.h"

class MainWindow;

class Depliage
{
public:
    Depliage(MainWindow*);

    QPointer<MainWindow> parent;
    DeplieurScene *scene3d = nullptr;
    DeplieurScene *scene2d = nullptr;
    QList<QGraphicsRectItem *> pages;
    QList<TriangleItem *> t2d;
    QGraphicsLineItem *flash = nullptr;
    Mesh *meshModel = nullptr;
    QList<int> sauveSel;
    QList<Pool> pool;
    //QFont tf;
    QList<Nums> nums;

    static constexpr float fPas = 0.15f;
    float fThetaX;
    float fThetaY;
    float fThetaZ;
    int dYt;
    float echelle = 1;

    QGraphicsRectItem* ajoutePage();
    void dessineModele();
    void creeFaces2d();
    void trouveVoisinage();
    void basculeSelectionChanged(bool);
};

int next(int);
int prev(int);

#endif // DEPLIAGE_H
