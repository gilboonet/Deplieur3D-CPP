//--------------------------------------------------//
// from https://www.youtube.com/watch?v=ih20l3pJoeU //
//--------------------------------------------------//
#include "mesh.h"

#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QDebug>

Mesh::Mesh() {}

bool Mesh::LoadFromObjectFile(const QByteArray &fdata)
{
    auto convsf = [](QString el) {
        QString s(el);
        size_t f = s.indexOf("/");
        if (f > 0) {
            s.truncate(f);
        }
        return s.toInt() - 1;
    };

    // Local cache of verts
    std::vector<vec3d> verts;

    QTextStream stream(fdata);
    QString line;
    int n = 0;

    vec3d vMin, vMax;

    line = stream.readLine();
    while (!line.isNull())
    {
        QStringList parts = line.split(" ");
        if (parts[0][0] == 'v')
        {            
            vec3d v (parts[1], parts[2], parts[3]);
            v = v.Vector_Div(50);
            verts.push_back(v);
            if (verts.size() == 1) {
                vMin = v;
                vMax = v;
            } else {
                if (v.x < vMin.x) vMin.x = v.x;
                if (v.y < vMin.y) vMin.y = v.y;
                if (v.z < vMin.z) vMin.z = v.z;

                if (v.x > vMax.x) vMax.x = v.x;
                if (v.y > vMax.y) vMax.y = v.y;
                if (v.z > vMax.z) vMax.z = v.z;
            }
        }

        if (parts[0][0] == 'f')
        {
            std::vector<int> pts;
            pts = {convsf(parts[1]), convsf(parts[2]), convsf(parts[3])};
            faces.push_back({
                verts[pts[0]], verts[pts[1]], verts[pts[2]],
                pts[0], pts[1], pts[2]
            });
        }
        line = stream.readLine();
    }
    verts.clear();

    dim = vMax.Vector_Sub(vMin);

    vec3d vDelta;
    vDelta = vMax.Vector_Add(vMin);
    vDelta = vDelta.Vector_Div(2.0f);
    for (auto&& t : faces) {
        for (auto && tp : t.p)
            tp = tp.Vector_Sub(vDelta);
        t.triangle2d = t.d2ize();
        t.id = n++;
        t.col = 0;
    }

    return true;
}

