#include "facette.h"

facette::facette() {}
facette::facette(vec3d a, vec3d b, vec3d c, int ai, int bi, int ci) {
    p[0] = a;
    p[1] = b;
    p[2] = c;
    pi[0] = ai;
    pi[1] = bi;
    pi[2] = ci;
}

Triangle2d facette::d2ize() {
    vec3d d1 = this->p[1].Vector_Sub(this->p[0]);
    vec3d d2 = this->p[2].Vector_Sub(this->p[0]);

    QPointF P0(0, 0);
    QPointF P1(sqrt((d1.x * d1.x) +(d1.y * d1.y) +(d1.z * d1.z)), 0);
    qreal P2x =((d1.x * d2.x) +(d1.y * d2.y) +(d1.z * d2.z)) / P1.x();
    QPointF P2(
        P2x,
        sqrt((d2.x * d2.x) +(d2.y * d2.y) +(d2.z * d2.z) -(P2x * P2x))
        );

    return Triangle2d({P0, P1, P2});
}

vec3d facette::point(const int n) {
    return p[n];
}

bool facette::eq3(facette t2, int n) {
    QVector3D pt = t2.p[n].toVector3D();

    bool r = pt.distanceToPoint(this->p[0].toVector3D()) >= 1.0f
             && pt.distanceToPoint(this->p[1].toVector3D()) >= 1.0f
             && pt.distanceToPoint(this->p[2].toVector3D()) >= 1.0f;

    return r;
}

qreal facette::isCoplanar(vec3d p) {
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
int facette::ClipAgainstPlane(vec3d plane_p, vec3d plane_n, facette& in_tri, facette& out_tri1, facette& out_tri2)
{
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

    if (nInsidePointCount == 0)
    {
        // All points lie on the outside of plane, so clip whole triangle
        // It ceases to exist

        return 0;   // No returned triangles are valid
    }

    if (nInsidePointCount == 3)
    {
        // All points lie on the inside of plane, so do nothing
        // and allow the triangle to simply pass through
        out_tri1 = in_tri;

        return 1;   // Just the one returned original triangle is valid
    }

    if (nInsidePointCount == 1 && nOutsidePointCount == 2)
    {
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

    if (nInsidePointCount == 2 && nOutsidePointCount == 1)
    {
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
}
