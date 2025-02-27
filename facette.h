// Données d'un triangle
//---------------------------------------------------------
#ifndef FACETTE_H
#define FACETTE_H

#include "vec3d.h"
#include "triangle2d.h"
#include "triangleitem2d.h"
#include "piecepolygonitem.h"

#include <array>
#include <QPolygonF>
#include <QByteArray>
#include <QColor>
//---------------------------------------------------------
class Ligne {
public:
    QPointF p1;
    QPointF p2;
    int id1;
    int id2;
    int cop;
    int nb;
    bool bLang;

    Ligne ();
    Ligne (QPointF, QPointF, int, int, int);
    bool operator== (const Ligne&) const;
};
//---------------------------------------------------------
class Voisin {
public:
    int id;
    int pnF;
    int nF;
    int idx;
    int cop;

    Voisin ();
    Voisin (int, int, int, int, int);
};
//---------------------------------------------------------
class Facette {
public:
    int id;                 // Id
    int col;                // Couleur

    std::array<vec3d, 3> p; // Coordonnées 3d
    int pi[3];              // id de leurs points
    Triangle2d triangle2d;  // Coordonnées 2d
    TriangleItem2d *triangleItem = nullptr;
    std::array<Voisin, 3> voisins;  // Voisins

    Facette ();
    Facette (vec3d, vec3d, vec3d, int, int, int);

    //int ClipAgainstPlane (vec3d, vec3d, Facette&, Facette&, Facette&);
    Triangle2d d2ize ();
    vec3d point (const int);

    qreal isCoplanar (vec3d);
    qreal isCoplanar (QVector3D);
    QPolygonF toPolygonF ();
    bool eq3 (Facette, int);
};

struct FacetteSimple {
    int id;
    int col;

    vec3d a;
    vec3d b;
    vec3d c;
    int ClipAgainstPlane (vec3d, vec3d,
                          FacetteSimple&, FacetteSimple&, FacetteSimple&);
};
//---------------------------------------------------------
struct Attache {
    int de;
    int vers;
    Attache();
    Attache(int);
    Attache(int, int);
    bool operator== (const Attache&) const;
};
//---------------------------------------------------------
struct Piece {
    int id;
    int nb;
    QColor couleur = QColor();
    QColor cDesign = QColor();
    QList<int> elements;
    QList<Attache> elements2;
    PiecePolygonItem *bord = nullptr;
    QGraphicsSimpleTextItem *num = nullptr;
    QList<Ligne> lignes;
    int hId = -1;

    void pieceConstruitBord ();
};
QList<QList<QPointF>> PtsDepuisLignesDeCoupe (Piece *piece);
//---------------------------------------------------------
enum TLang {
    L00,
    L10,
    L01,
    L11
};
//---------------------------------------------------------
class Nums {
public :
    int id1;
    int id2;
    int num;
    enum TLang tlang = L00;
    Nums ();
    Nums (int, int);
    Nums (int, int, int);
    bool operator== (const Nums&) const;
};

#endif // FACETTE_H
