// Données d'un triangle
//---------------------------------------------------------
#include "facette.h"

#include <QString>
#include <QStringList>
#include <QTextStream>
//---------------------------------------------------------
Voisin::Voisin () {}
Voisin::Voisin (int id, int pnF, int nF, int idx, int cop)
    : id(id), pnF(pnF), nF(nF), idx(idx), cop(cop) {}
//---------------------------------------------------------
Ligne::Ligne () {}
Ligne::Ligne(QPointF p1, QPointF p2, int id1, int id2, int cop)
    : p1(p1), p2(p2), id1(id1), id2(id2), cop(cop), nb(1) {}

bool Ligne::operator==(const Ligne &l) const
{
    return ((this->id1 == l.id1) && (this->id2 == l.id2));
}
//---------------------------------------------------------
Nums::Nums () {}
Nums::Nums (int n1, int n2) : id1(n1), id2(n2) {}
Nums::Nums (int n1, int n2, int num) : id1(n1), id2(n2), num(num) {}
bool Nums::operator==(const Nums &n) const {
    return
        ((this->id1 == n.id1) && (this->id2 == n.id2))
     || ((this->id1 == n.id2) && (this->id2 == n.id1));
}
//---------------------------------------------------------
Attache::Attache() {}
Attache::Attache(int vers) : vers(vers) {}
Attache::Attache(int de, int vers) : de(de), vers(vers) {}
bool Attache::operator==(const Attache &a) const
{
    return this->vers == a.vers;
}
//---------------------------------------------------------
QList<QList<QPointF>> PtsDepuisLignesDeCoupe(Piece *piece) {
    QList<QLineF> lLignes;
    for (auto&& e : piece->lignes) {
        if (e.nb == 1)
            lLignes << QLineF (e.p1, e.p2);
    }

    QList<QList<QPointF>> liste;

    while (lLignes.size() > 0) {
        QPointF premPoint = lLignes.first().p1();
        QPointF dernPoint = lLignes.first().p2();
        QList<QPointF> lPoints;
        lPoints << premPoint;
        lPoints << dernPoint;
        lLignes.removeFirst();

        while ((lLignes.size() > 0) && !eq(premPoint, dernPoint)) {
            for (QList<QLineF>::const_iterator i = lLignes.constBegin(); i != lLignes.constEnd(); i++) {
                QLineF l = *i;
                if (eq(l.p1(), premPoint)) {
                    premPoint = l.p2();
                    lPoints.prepend(premPoint);
                    lLignes.erase(i);
                    break;
                } else if (eq(l.p2(), premPoint)) {
                    premPoint = l.p1();
                    lPoints.prepend(premPoint);
                    lLignes.erase(i);
                    break;
                } else if (eq(l.p1(), dernPoint)) {
                    dernPoint = l.p2();
                    lPoints.append(dernPoint);
                    lLignes.erase(i);
                    break;
                } else if (eq(l.p2(), dernPoint)) {
                    dernPoint = l.p1();
                    lPoints.append(dernPoint);
                    lLignes.erase(i);
                    break;
                }
            }
        }
        liste.push_back(lPoints);
    }
    return liste;
}
//---------------------------------------------------------
void Piece::pieceConstruitBord () {
    // construction du polygone
    QList<QList<QPointF>> lPoints = PtsDepuisLignesDeCoupe (this);
    QList<QPolygonF> qpolys;
    QPolygonF qpoly = QPolygon();
    int nPlusGrand = 0;
    int i = 0;
    for (auto&& lp : lPoints) {
        QPolygonF p = QPolygonF(lp);
        qpolys.push_back(p);
        QRectF bPoly = qpoly.boundingRect();
        QRectF pPoly = p.boundingRect();
        if ((pPoly.width() > bPoly.width()) || (pPoly.height() > bPoly.height())) {
            qpoly = p;
            nPlusGrand = i;
        }
        i++;
    }

    i=0;
    for (auto&& qp : qpolys) {
        if (i != nPlusGrand) {
            qpoly = qpoly.subtracted(qp);
        }
        i++;
    }

    bord->setPolygon(qpoly);
}
//---------------------------------------------------------
Facette::Facette() {
    id = -1;
    col = -1;
}

Facette::Facette (vec3d a, vec3d b, vec3d c, int ai, int bi, int ci) {
    p[0] = a;
    p[1] = b;
    p[2] = c;
    pi[0] = ai;
    pi[1] = bi;
    pi[2] = ci;
}

Triangle2d Facette::d2ize () {
    vec3d d1 = this->p[1].Vector_Sub(this->p[0]);
    vec3d d2 = this->p[2].Vector_Sub(this->p[0]);

    QPointF P0(0, 0);
    QPointF P1(sqrt((d1.x * d1.x) +(d1.y * d1.y) +(d1.z * d1.z)), 0);
    qreal P2x =((d1.x * d2.x) +(d1.y * d2.y) +(d1.z * d2.z)) / P1.x();
    QPointF P2(
        P2x,
        sqrt((d2.x * d2.x) +(d2.y * d2.y) +(d2.z * d2.z) -(P2x * P2x))
        );

    return Triangle2d ({P0, P1, P2});
}

/*QPolygon Facette::toPolygon () {
    return QPolygon ({p[0].toPoint(), p[1].toPoint(), p[2].toPoint()});
}*/

QPolygonF Facette::toPolygonF () {
    return QPolygonF ({p[0].toPointF(), p[1].toPointF(), p[2].toPointF()});
}

vec3d Facette::point (const int n) {
    return p[n];
}

bool Facette::eq3 (Facette t2, int n) {
    QVector3D pt = t2.p[n].toVector3D();

    bool r = pt.distanceToPoint(this->p[0].toVector3D()) >= 1.0f
             && pt.distanceToPoint(this->p[1].toVector3D()) >= 1.0f
             && pt.distanceToPoint(this->p[2].toVector3D()) >= 1.0f;

    return r;
}

qreal Facette::isCoplanar (vec3d p) {
    vec3d v1 = this->p[1].Vector_Sub(this->p[0]);
    vec3d v2 = this->p[2].Vector_Sub(this->p[0]);

    qreal a = v1.y * v2.z - v2.y * v1.z;
    qreal b = v2.x * v1.z - v1.x * v2.z;
    qreal c = v1.x * v2.y - v1.y * v2.x;
    qreal d = -(a * this->p[0].x)
              -(b * this->p[0].y)
              -(c * this->p[0].z);

    qreal r = (a * p.x) +(b * p.y) +(c * p.z) + d;

    return r;
}

//--------------------------------------------------//
// from https://www.youtube.com/watch?v=ih20l3pJoeU //
//--------------------------------------------------//
int FacetteSimple::ClipAgainstPlane (vec3d plane_p, vec3d plane_n,
                                    FacetteSimple& in_tri, FacetteSimple& out_tri1, FacetteSimple& out_tri2) {
    // Make sure plane normal is indeed normal
    plane_n = plane_n.Vector_Normalise();

    // Return signed shortest distance from point to plane, plane normal must be normalised
    auto dist = [&](vec3d& p)
    {
        vec3d n = p.Vector_Normalise();
        return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - plane_n.Vector_DotProduct(plane_p));
    };

    // Create two temporary storage arrays to classify points either side of plane
    // If distance sign is positive, point lies on "inside" of plane
    vec3d* inside_points[3];  int nInsidePointCount = 0;
    vec3d* outside_points[3]; int nOutsidePointCount = 0;

    // Get signed distance of each point in triangle to plane
    float d0 = dist(in_tri.a);
    float d1 = dist(in_tri.b);
    float d2 = dist(in_tri.c);

    if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.a; }
    else { outside_points[nOutsidePointCount++] = &in_tri.a; }
    if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.b; }
    else { outside_points[nOutsidePointCount++] = &in_tri.b; }
    if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.c; }
    else { outside_points[nOutsidePointCount++] = &in_tri.c; }

    // Now classify triangle points, and break the input triangle into
    // smaller output triangles if required. There are four possible
    // outcomes...

    if (nInsidePointCount == 0) {
        // All points lie on the outside of plane, so clip whole triangle
        // It ceases to exist

        return 0;   // No returned triangles are valid
    }

    if (nInsidePointCount == 3) {
        // All points lie on the inside of plane, so do nothing
        // and allow the triangle to simply pass through
        out_tri1 = in_tri;

        return 1;   // Just the one returned original triangle is valid
    }

    if (nInsidePointCount == 1 && nOutsidePointCount == 2) {
        // Triangle should be clipped. As two points lie outside
        // the plane, the triangle simply becomes a smaller triangle

        // Copy appearance info to new triangle
        out_tri1.id = in_tri.id;
        out_tri1.col = in_tri.col;

        // The inside point is valid, so keep that...
        out_tri1.a = *inside_points[0];

        // but the two new points are at the locations where the
        // original sides of the triangle (lines) intersect with the plane
        out_tri1.b = plane_p.Vector_IntersectPlane(plane_n, *inside_points[0], *outside_points[0]);
        out_tri1.c = plane_p.Vector_IntersectPlane(plane_n, *inside_points[0], *outside_points[1]);

        return 1; // Return the newly formed single triangle
    }

    if (nInsidePointCount == 2 && nOutsidePointCount == 1) {
        // Triangle should be clipped. As two points lie inside the plane,
        // the clipped triangle becomes a "quad". Fortunately, we can
        // represent a quad with two new triangles

        // Copy appearance info to new triangles
        out_tri1.col = in_tri.col;
        out_tri1.id = in_tri.id;

        out_tri2.col = in_tri.col;
        out_tri2.id = in_tri.id;

        // The first triangle consists of the two inside points and a new
        // point determined by the location where one side of the triangle
        // intersects with the plane
        out_tri1.a = *inside_points[0];
        out_tri1.b = *inside_points[1];
        out_tri1.c = plane_p.Vector_IntersectPlane(plane_n, *inside_points[0], *outside_points[0]);

        // The second triangle is composed of one of he inside points, a
        // new point determined by the intersection of the other side of the
        // triangle and the plane, and the newly created point above
        out_tri2.a = *inside_points[1];
        out_tri2.b = out_tri1.c;
        out_tri2.c = plane_p.Vector_IntersectPlane(plane_n, *inside_points[1], *outside_points[0]);

        return 2;   // Return two newly formed triangles which form a quad
    }

    return 0;
}

/*int Facette::ClipAgainstPlane (vec3d plane_p, vec3d plane_n, Facette& in_tri, Facette& out_tri1, Facette& out_tri2) {
    // Make sure plane normal is indeed normal
    plane_n = plane_n.Vector_Normalise();

    // Return signed shortest distance from point to plane, plane normal must be normalised
    auto dist = [&](vec3d& p)
    {
        vec3d n = p.Vector_Normalise();
        return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - plane_n.Vector_DotProduct(plane_p));
    };

    // Create two temporary storage arrays to classify points either side of plane
    // If distance sign is positive, point lies on "inside" of plane
    vec3d* inside_points[3];  int nInsidePointCount = 0;
    vec3d* outside_points[3]; int nOutsidePointCount = 0;

    // Get signed distance of each point in triangle to plane
    float d0 = dist(in_tri.p[0]);
    float d1 = dist(in_tri.p[1]);
    float d2 = dist(in_tri.p[2]);

    if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[0]; }
    else { outside_points[nOutsidePointCount++] = &in_tri.p[0]; }
    if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[1]; }
    else { outside_points[nOutsidePointCount++] = &in_tri.p[1]; }
    if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[2]; }
    else { outside_points[nOutsidePointCount++] = &in_tri.p[2]; }

    // Now classify triangle points, and break the input triangle into
    // smaller output triangles if required. There are four possible
    // outcomes...

    if (nInsidePointCount == 0) {
        // All points lie on the outside of plane, so clip whole triangle
        // It ceases to exist

        return 0;   // No returned triangles are valid
    }

    if (nInsidePointCount == 3) {
        // All points lie on the inside of plane, so do nothing
        // and allow the triangle to simply pass through
        out_tri1 = in_tri;

        return 1;   // Just the one returned original triangle is valid
    }

    if (nInsidePointCount == 1 && nOutsidePointCount == 2) {
        // Triangle should be clipped. As two points lie outside
        // the plane, the triangle simply becomes a smaller triangle

        // Copy appearance info to new triangle
        out_tri1.col = in_tri.col;

        // The inside point is valid, so keep that...
        out_tri1.p[0] = *inside_points[0];

        // but the two new points are at the locations where the
        // original sides of the triangle (lines) intersect with the plane
        out_tri1.p[1] = plane_p.Vector_IntersectPlane(plane_n, *inside_points[0], *outside_points[0]);
        out_tri1.p[2] = plane_p.Vector_IntersectPlane(plane_n, *inside_points[0], *outside_points[1]);

        return 1; // Return the newly formed single triangle
    }

    if (nInsidePointCount == 2 && nOutsidePointCount == 1) {
        // Triangle should be clipped. As two points lie inside the plane,
        // the clipped triangle becomes a "quad". Fortunately, we can
        // represent a quad with two new triangles

        // Copy appearance info to new triangles
        out_tri1.col = in_tri.col;

        out_tri2.col = in_tri.col;

        // The first triangle consists of the two inside points and a new        
        // point determined by the location where one side of the triangle
        // intersects with the plane
        out_tri1.p[0] = *inside_points[0];
        out_tri1.p[1] = *inside_points[1];
        out_tri1.p[2] = plane_p.Vector_IntersectPlane(plane_n, *inside_points[0], *outside_points[0]);

        // The second triangle is composed of one of he inside points, a
        // new point determined by the intersection of the other side of the
        // triangle and the plane, and the newly created point above
        out_tri2.p[0] = *inside_points[1];
        out_tri2.p[1] = out_tri1.p[2];
        out_tri2.p[2] = plane_p.Vector_IntersectPlane(plane_n, *inside_points[1], *outside_points[0]);

        return 2;   // Return two newly formed triangles which form a quad
    }

    return 0;
}*/
