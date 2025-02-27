//---------------------------------------------------------
// from https://www.youtube.com/watch?v=ih20l3pJoeU
//---------------------------------------------------------
#include "vec3d.h"

#include <qmath.h>
//---------------------------------------------------------
vec3d::vec3d () {}
vec3d::vec3d (float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
vec3d::vec3d (float x, float y, float z) : x(x), y(y), z(z), w(1) {}

vec3d::vec3d (QString sX, QString sY, QString sZ) : x(sX.toFloat()), y(sY.toFloat()), z(sZ.toFloat()), w(1) {}

QDebug operator<< (QDebug d, const vec3d &v) {
    d << v.x << ", " << v.y << ", " << v.z;
    return d;
}

vec3d vec3d::Vector_Add (vec3d& v2) {
    return { this->x + v2.x, this->y + v2.y, this->z + v2.z };
}

vec3d vec3d::Vector_Sub (vec3d& v2) {
    return { this->x - v2.x, this->y - v2.y, this->z - v2.z };
}

vec3d vec3d::Vector_Mul (float k) {
    return { this->x * k, this->y * k, this->z * k };
}

vec3d vec3d::Vector_Div (float k) {
    return { this->x / k, this->y / k, this->z / k };
}

float vec3d::Vector_DotProduct (vec3d& v2) {
    return this->x * v2.x + this->y * v2.y + this->z * v2.z;
}

float vec3d::Vector_Length () {
    //return 1.0f;
    return sqrtf(this->Vector_DotProduct(*this));
}

vec3d vec3d::Vector_Normalise () {
    float l = this->Vector_Length();
    return { this->x / l, this->y / l, this->z / l };
}

vec3d vec3d::Vector_CrossProduct (vec3d& v2) {
    vec3d v;
    v.x = this->y * v2.z - this->z * v2.y;
    v.y = this->z * v2.x - this->x * v2.z;
    v.z = this->x * v2.y - this->y * v2.x;
    return v;
}

vec3d vec3d::Vector_IntersectPlane (vec3d& plane_n, vec3d& lineStart, vec3d& lineEnd) {
    plane_n = plane_n.Vector_Normalise();
    float plane_d = - plane_n.Vector_DotProduct(*this);
    float ad = lineStart.Vector_DotProduct(plane_n);
    float bd = lineEnd.Vector_DotProduct(plane_n);
    float t = (-plane_d - ad) / (bd - ad);
    vec3d lineStartToEnd = lineEnd.Vector_Sub(lineStart);
    vec3d lineToIntersect = lineStartToEnd.Vector_Mul(t);
    return lineStart.Vector_Add(lineToIntersect);
}

QPointF vec3d::toPointF () {
    return QPointF(this->x, this->y);
}

QPoint vec3d::toPoint () {
    return QPoint(this->x, this->y);
}

QVector3D vec3d::toVector3D () {
    return QVector3D(this->x, this->y, this->z);
}
