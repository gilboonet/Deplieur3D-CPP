// Données du projet de dépliage
//---------------------------------------------------------
#include "depliage.h"
#include "mat4x4.h"
#include "triangleitem3d.h"
//---------------------------------------------------------
// Constructeur
Depliage::Depliage () {
    fThetaX = 0;
    fThetaY = 0;
    fThetaZ = 0;
    dYt = 0;
    faces.clear();
    prochainNum = 0;
    nums.clear();
    dp.clear();
    echelle = 1;
    ModeleOK = false;
    typeLang = 0;
    dimPage = QPoint(210, 297);
}

void Depliage::dessineModele (DepliageScene *scene3d) {
    if (!faces.empty()) {
        scene3d->clear();

        // Set up rotation matrices
        mat4x4 matRotZ, matRotY, matRotX;

        // Projection Matrix
        mat4x4 matProj;
        matProj = matProj.Matrix_MakeProjection(90.0f, 1.0f, 0.1f, 2000.0f);

        matRotZ = matRotZ.Matrix_MakeRotationZ(fThetaZ);
        matRotY = matRotY.Matrix_MakeRotationY(fThetaY);
        matRotX = matRotX.Matrix_MakeRotationX(fThetaX);

        mat4x4 matTrans;
        matTrans = matTrans.Matrix_MakeTranslation(0.0f, 0.0f, 5.0f);

        mat4x4 matWorld;
        matWorld = matWorld.Matrix_MakeIdentity();	// Form World Matrix
        matWorld = matWorld.Matrix_MultiplyMatrix(matRotZ, matRotY); // Transform by rotation
        matWorld = matWorld.Matrix_MultiplyMatrix(matWorld, matRotX);
        matWorld = matWorld.Matrix_MultiplyMatrix(matWorld, matTrans); // Transform by translation

        vec3d vCamera(0,0,0);
        vec3d vLookDir(0,0,0);

        // Create "Point At" Matrix for camera
        vec3d vUp(0,1,0);
        vec3d vTarget(0,0,1);
        mat4x4 matCameraRot;
        matCameraRot = matCameraRot.Matrix_MakeIdentity();
        vLookDir = matCameraRot.Matrix_MultiplyVector(matCameraRot, vTarget);
        vTarget = vCamera.Vector_Add(vLookDir);
        mat4x4 matCamera;
        matCamera = matCamera.Matrix_PointAt(vCamera, vTarget, vUp);

        // Make view matrix from camera
        mat4x4 matView;
        matView = matView.Matrix_QuickInverse(matCamera);

        // Store triagles for rastering later
        QList<Facette> vecTrianglesToRaster;

        // Draw Triangles
        for (auto &tri : faces) {
            Facette triProjected, triTransformed, triViewed;

            // World Matrix Transform
            triTransformed = tri;
            triTransformed.p[0] = matWorld.Matrix_MultiplyVector(matWorld, tri.p[0]);
            triTransformed.p[1] = matWorld.Matrix_MultiplyVector(matWorld, tri.p[1]);
            triTransformed.p[2] = matWorld.Matrix_MultiplyVector(matWorld, tri.p[2]);
            //triTransformed.col = tri.col;
            //triTransformed.id = tri.id;

            // Calculate triangle Normal
            vec3d normal, line1, line2;

            // Get lines either side of triangle
            line1 = triTransformed.p[1].Vector_Sub(triTransformed.p[0]);
            line2 = triTransformed.p[2].Vector_Sub(triTransformed.p[0]);

            // Take cross product of lines to get normal to triangle surface
            normal = line1.Vector_CrossProduct(line2);

            // You normally need to normalise a normal!
            normal = normal.Vector_Normalise();

            // Get Ray from triangle to camera
            vec3d vCameraRay = triTransformed.p[0].Vector_Sub(vCamera);
            // If ray is aligned with normal, then triangle is visible
            if (normal.Vector_DotProduct(vCameraRay) < 0.0f) {
                // Illumination
                vec3d light_direction = { 0.0f, 1.0f, -1.0f };
                light_direction = light_direction.Vector_Normalise();

                // How "aligned" are light direction and triangle surface normal?
                //float dp = std::max(0.1f, Vector_DotProduct(light_direction, normal));

                // Convert World Space --> View Space
                triViewed = triTransformed;
                triViewed.p[0] = matView.Matrix_MultiplyVector(matView, triTransformed.p[0]);
                triViewed.p[1] = matView.Matrix_MultiplyVector(matView, triTransformed.p[1]);
                triViewed.p[2] = matView.Matrix_MultiplyVector(matView, triTransformed.p[2]);
                //triViewed.col = triTransformed.col;
                //triViewed.id = triTransformed.id;

                // Clip Viewed Triangle against near plane, this could form two additional
                // additional triangles.
                int nClippedTriangles = 0;
                Facette clipped[2];
                nClippedTriangles = clipped[0].ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f },
                                                                triViewed, clipped[0], clipped[1]);

                // We may end up with multiple triangles from the clip, so project as
                // required
                for (int n = 0; n < nClippedTriangles; n++) {
                    // Project triangles from 3D --> 2D
                    triProjected = triViewed;
                    triProjected.p[0] = matProj.Matrix_MultiplyVector(matProj, clipped[n].p[0]);
                    triProjected.p[1] = matProj.Matrix_MultiplyVector(matProj, clipped[n].p[1]);
                    triProjected.p[2] = matProj.Matrix_MultiplyVector(matProj, clipped[n].p[2]);
                    //triProjected.col = clipped[n].col;
                    //triProjected.id = clipped[n].id;

                    // Scale into view, we moved the normalising into cartesian space
                    // out of the matrix.vector function from the previous videos, so
                    // do this manually
                    triProjected.p[0] = triProjected.p[0].Vector_Div(triProjected.p[0].w);
                    triProjected.p[1] = triProjected.p[1].Vector_Div(triProjected.p[1].w);
                    triProjected.p[2] = triProjected.p[2].Vector_Div(triProjected.p[2].w);

                    // X/Y are inverted so put them back
                    triProjected.p[0].x *= -1.0f;
                    triProjected.p[1].x *= -1.0f;
                    triProjected.p[2].x *= -1.0f;
                    triProjected.p[0].y *= -1.0f;
                    triProjected.p[1].y *= -1.0f;
                    triProjected.p[2].y *= -1.0f;

                    // Offset verts into visible normalised space
                    vec3d vOffsetView = { 1,1,0 };
                    triProjected.p[0] = triProjected.p[0].Vector_Add(vOffsetView);
                    triProjected.p[1] = triProjected.p[1].Vector_Add(vOffsetView);
                    triProjected.p[2] = triProjected.p[2].Vector_Add(vOffsetView);
                    triProjected.p[0].x *= 1000.0f;
                    triProjected.p[0].y *= 1000.0f;
                    triProjected.p[1].x *= 1000.0f;
                    triProjected.p[1].y *= 1000.0f;
                    triProjected.p[2].x *= 1000.0f;
                    triProjected.p[2].y *= 1000.0f;

                    // Store triangle for sorting
                    vecTrianglesToRaster.push_back(triProjected);
                }
            }
        }

        // Sort triangles from back to front
        std::sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](Facette& t1, Facette& t2)
        {
            float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
            float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
            return z1 > z2;
        });

        // Loop through all transformed, viewed, projected, and sorted triangles
        for (auto& triToRaster : vecTrianglesToRaster) {
            // Clip triangles against all four screen edges, this could yield
            // a bunch of triangles, so create a queue that we traverse to
            //  ensure we only test new triangles generated against planes
            Facette clipped[2];
            std::list<Facette> listTriangles;

            // Add initial triangle
            listTriangles.push_back(triToRaster);
            size_t nNewTriangles = 1;

            for (int p = 0; p < 4; p++) {
                int nTrisToAdd = 0;
                while (nNewTriangles > 0)
                {
                    // Take triangle from front of queue
                    Facette test = listTriangles.front();
                    listTriangles.pop_front();
                    nNewTriangles--;

                    // Clip it against a plane. We only need to test each
                    // subsequent plane, against subsequent new triangles
                    // as all triangles after a plane clip are guaranteed
                    // to lie on the inside of the plane. I like how this
                    // comment is almost completely and utterly justified
                    clipped[0] = triToRaster;
                    switch (p)
                    {
                    case 0: nTrisToAdd = clipped[0].ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                    case 1:	nTrisToAdd = clipped[0].ClipAgainstPlane({ 0.0f, 1999.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                    case 2:	nTrisToAdd = clipped[0].ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                    case 3:	nTrisToAdd = clipped[0].ClipAgainstPlane({ 1999.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                    }

                    // Clipping may yield a variable number of triangles,
                    // so add these new ones to the back of the queue for
                    // subsequent clipping against next planes
                    for (int w = 0; w < nTrisToAdd; w++) {
                        listTriangles.push_back(clipped[w]);
                    }
                }
                nNewTriangles = listTriangles.size();
            }

            // Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
            for (auto& t : listTriangles) {
                TriangleItem3d *ti = new TriangleItem3d(scene3d, pieces[t.col].couleur,
                                                        t.toPolygon(), t.id, t.col);
                ti->setVisible(true);
            }
            scene3d->update();
        }
    }
}

void Depliage::creeFaces2d (DepliageScene *scene2d) {
    // Crée les facettes 2d servant pour le gabarit
    QTransform transform;
    qreal px2cm = 50.0f;
    transform.scale(px2cm, px2cm);

    scene2d->clear();
    scene2d->pageTemoin = nullptr;
    for (auto&& face : faces) {
        QPolygonF poly = transform.map(face.triangle2d.toPolygon());
        poly.translate(-poly.boundingRect().left(), -poly.boundingRect().top());
        delete face.triangleItem;
        face.triangleItem = new TriangleItem2d(scene2d, pieces[face.col].couleur , poly, face.id, face.col);
    }
    scene2d->update();
}

bool Depliage::chargeFichierOBJ (const QByteArray &fdata, bool estProjet) {
    auto convsf = [](QString el) -> int
    {
        QString s(el);
        qsizetype f = s.indexOf("/");
        if (f > -1) {
            s.truncate(f);
        }
        return s.toInt() - 1;
    };

    // Local cache of verts
    QList<vec3d> verts;

    QTextStream stream(fdata);
    QString line;
    int n = 0;

    vec3d vMin, vMax;

    line = stream.readLine();
    while (!line.isNull()) {
        QStringList parts = line.split(" ");

        if (parts[0] == "v") {
            vec3d v (parts[1], parts[2], parts[3]);
            v = v.Vector_Div(50);
            verts.append(v);
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

        else if (parts[0] == "f") {
            std::vector<int> pts;
            pts = {convsf(parts[1]), convsf(parts[2]), convsf(parts[3])};
            faces.append({
                verts[pts[0]], verts[pts[1]], verts[pts[2]],
                pts[0], pts[1], pts[2]
            });
        }

        else if (parts[0] == "dp") {
            dp.append(line);

        }

        else if (parts[0] == "de") {
            dp.append(line);

        }

        else if (parts[0] == "d1") {
            dp.append(line);

        }

        else if (parts[0] == "dl") {
            dp.append(line);

        }

        else if (parts[0] == "dn") {
            dp.append(line);

        }

        else if (parts[0] == "dd") {
            dp.append(line);

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

    qreal mx = std::max({dim.x, dim.y, dim.z});
    qDebug() << "max : " << mx;
    if ((mx < 4) || (mx > 10)) {
        qreal delta = 4 / mx;
        for (auto && v : faces) {
                for (auto && vp : v.p)
                    vp =  vp.Vector_Mul(delta);
        }
    }

    return true;
}

void Depliage::trouveVoisinage () {
    int i;
    int vi;
    bool ok;
    int nbFaces = faces.size();

    for (auto &&ti : faces) {
        i = ti.id;
        for (int j = 0; j < 3; j++) {
            vi = 0;
            ok = false;
            do {
                if (vi != i) {
                    for (int k = 0; (k < 3) && !ok; k++)
                        if ( (faces[vi].pi[k] == ti.pi[next(j)])
                            && (faces[vi].pi[next(k)] == ti.pi[j]) ) {

                            // calcule coplanéité
                            int p;
                            if (faces[vi].eq3(ti, 0))
                                p = 0;
                            else if(faces[vi].eq3(ti, 1))
                                p = 1;
                            else
                                p = 2;

                            qreal c = faces[vi].isCoplanar(ti.p[p]);
                            ti.voisins[j] = Voisin(j, i, vi, next(k), sgn(c));
                            ok = true;
                        }
                    if (!ok)
                        vi++;
                } else
                    vi++;
            } while ((vi < nbFaces) && !ok);
        }
    }
}

int Depliage::chercheNum (int id1, int id2) {
    Nums num1 = Nums(id1, id2);
    Nums num2 = Nums(id2, id1);
    int numC;
    if (this->nums.contains(num1)) {
        numC = this->nums[this->nums.indexOf(num1)].num;
        if (numC == -1) {
            numC = prochainNum++;
            this->nums[this->nums.indexOf(num1)].num = numC;
        }
    } else {
        numC = prochainNum++;
        num1.num = numC;
        num1.tlang = (typeLang == 0) ? L00 : (typeLang == 2) ? L11 : L10;
        this->nums.append(num1);

    }
    return numC;
}
//---------------------------------------------------------
// next et prev retournent le prochain/précédent dans
// une liste contenant [0,1,2] en bouclant si besoin
int next (int n) { return n == 2 ? 0 : n+1; }
int prev (int n) { return n == 0 ? 2 : n-1; }

int sgn (qreal x) {
    if (x < 0) return -1;
    if (x > 0) return 1;
    return 0;
}
