#ifndef MESH_H
#define MESH_H

#include "facette.h"
#include <QByteArray>

class Mesh
{
public:
    std::vector<facette> faces;

    Mesh();

    bool LoadFromObjectFile(const QByteArray &fdata);
};

#endif // MESH_H
