#ifndef VEC3D_H
#define VEC3D_H

#include <QDebug>
#include <QVector3D>

class vec3d
{
public:
    float x;
    float y;
    float z;
    float w;

    vec3d();
    vec3d(float, float, float, float);
    vec3d(float, float, float);
    vec3d(QString, QString, QString);

    vec3d Vector_Add(vec3d&);
    vec3d Vector_Sub(vec3d&);
    vec3d Vector_Mul(float);
    vec3d Vector_Div(float);
    float Vector_DotProduct(vec3d&);
    float Vector_Length();
    vec3d Vector_Normalise();
    vec3d Vector_CrossProduct(vec3d&);
    vec3d Vector_IntersectPlane(vec3d&, vec3d&, vec3d&);
    QPointF toPointF();
    QVector3D toVector3D();

    friend QDebug operator<< (QDebug d, const vec3d& v);
};

#endif // VEC3D_H
