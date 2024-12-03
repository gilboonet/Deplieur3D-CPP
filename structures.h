#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QGraphicsLineItem>

struct Attache {
    int de;     // -1 si première face
    int vers;
};

class Ligne {
public :
    QPointF p1;
    QPointF p2;
    int nb;
    int id1;
    int id2;
    int cop;
    QGraphicsLineItem *li;
    Ligne();
    Ligne(QPointF, QPointF, int, int, int);
};

struct Piece {
    QList<Attache> attaches;
    QList<Ligne> lignes;
    int premId;
};

struct Pool {
    QColor couleur;
    QList<int> elements;
    QList<Piece> pieces;
};

class Nums {
public :
    int id1;
    int id2;
    int num;
    Nums();
    Nums(int, int);
    Nums(int, int, int);
    bool operator==(const Nums&) const;
};



#endif // STRUCTURES_H

