#ifndef MAT4X4_H
#define MAT4X4_H

#include "vec3d.h"

class mat4x4
{
public:
    float m[4][4] = { 0 };
    mat4x4();

    vec3d Matrix_MultiplyVector(mat4x4&, vec3d&);
    mat4x4 Matrix_MakeIdentity();
    mat4x4 Matrix_MakeRotationX(float);
    mat4x4 Matrix_MakeRotationY(float);
    mat4x4 Matrix_MakeRotationZ(float);
    mat4x4 Matrix_MakeTranslation(float, float, float);
    mat4x4 Matrix_MakeProjection(float, float, float, float);
    mat4x4 Matrix_MultiplyMatrix(mat4x4&, mat4x4&);
    mat4x4 Matrix_PointAt(vec3d&, vec3d&, vec3d&);
    mat4x4 Matrix_QuickInverse(mat4x4&);
};

#endif // MAT4X4_H
