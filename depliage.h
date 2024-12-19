// Données du projet de dépliage
//---------------------------------------------------------
#ifndef DEPLIAGE_H
#define DEPLIAGE_H

#include "facette.h"
#include "depliagescene.h"
//---------------------------------------------------------
class Facette;
class Piece;
class DepliageScene;
//---------------------------------------------------------
class Depliage
{
public:
    Depliage();

    QList<Facette> faces;
    vec3d dim; // Dimensions courantes du modèle
    QList<Piece> pieces;
    int prochainNum = 0;
    QList<Nums> nums;
    qreal echelle = 1;
    static constexpr float fPas = 0.15f;
    float fThetaX;
    float fThetaY;
    float fThetaZ;
    int dYt;
    bool ModeleOK = false;
    int typeLang; // 0, 1 ou 2
    QPoint dimPage;

    void dessineModele (DepliageScene *);
    void creeFaces2d (DepliageScene *);
    bool chargeFichierOBJ (const QByteArray &fdata);
    void trouveVoisinage ();
    int chercheNum (int id1, int id2);
};
//---------------------------------------------------------
int next (int);
int prev (int);
int sgn (qreal);

#endif // DEPLIAGE_H
