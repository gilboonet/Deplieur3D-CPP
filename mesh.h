#ifndef MESH_H
#define MESH_H

#include "facette.h"
#include <QByteArray>

class Mesh
{
public:
    std::vector<facette> faces;
    vec3d dim;

    Mesh();

    bool LoadFromObjectFile(const QByteArray &fdata);
};

#endif // MESH_H
