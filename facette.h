#ifndef FACETTE_H
#define FACETTE_H

#include "vec3d.h"
#include <array>
#include "voisin.h"
#include "triangle2d.h"


/*struct Triangle2d {
    QPointF a, b, c;
    Triangle2d () {}
    Triangle2d (QPointF va, QPointF vb, QPointF vc) : a(va), b(vb), c(vc) {}
};*/

class facette {
public:
    int id;               // Id
    int col;               // Couleur
    vec3d p[3];                // Coordonnées 3d
    int pi[3];                 // id de leurs points
    Triangle2d triangle2d;     // Coordonnées 2d
    std::array<Voisin, 3> voisins;  // Voisins
    bool estPremFace;          // Vrai si 1ère face d'une pièce
    bool estLie;               // Vrai si est attaché à une face (dans une pièce)

    facette();
    facette(vec3d, vec3d, vec3d, int, int, int);

    int ClipAgainstPlane(vec3d, vec3d, facette&, facette&, facette&);
    Triangle2d d2ize();
    vec3d point(const int);
    bool eq3(facette, int);
    qreal isCoplanar(vec3d);
    qreal isCoplanar(QVector3D);

};
#endif // FACETTE_H
