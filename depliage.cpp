#include "depliage.h"

Ligne::Ligne() {}
Ligne::Ligne(QPointF p1, QPointF p2, int id1, int id2, int cop) :
    p1(p1), p2(p2), id1(id1), id2(id2), cop(cop), nb(1)
{}

Depliage::Depliage(MainWindow* p)
{   // CONSTRUCTEUR
    parent = QPointer<MainWindow>(p);
    if (scene3d == nullptr) {
        scene3d = new DeplieurScene(parent);
    }
    if (scene2d == nullptr) {
        scene2d = new DeplieurScene(parent);
    }
    meshModel = new Mesh;

    fThetaX = 0.0f;
    fThetaY = 0.0f;
    fThetaZ = 0.0f;

#ifdef Q_OS_WASM
    dYt = -2;
#else
    dYt = -1;
#endif

    tf = QFont("Bitstream Vera Sans", 8);
    tf.setLetterSpacing(QFont::AbsoluteSpacing, -2);
}

void Depliage::dessineModele()
{
    if (!meshModel->faces.empty())
    {
        // sauver selection
        sauveSel.clear();
        for (auto&& s: scene2d->selectedItems()) {
            int i = s->data(0).toInt();
            sauveSel.push_back(i);
        }

        scene3d->clear();

        // Set up rotation matrices
        mat4x4 matRotZ, matRotY, matRotX;

        // Projection Matrix
        mat4x4 matProj;
        matProj = matProj.Matrix_MakeProjection(90.0f, 1.0f, 0.1f, 1200.0f);

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
        std::vector<facette> vecTrianglesToRaster;

        // Draw Triangles
        for (auto &tri : meshModel->faces)
        {
            facette triProjected, triTransformed, triViewed;

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
            //vec3d vCameraRay = triTransformed.p[0].Vector_Sub(vCamera);
            // If ray is aligned with normal, then triangle is visible
            //if (normal.Vector_DotProduct(vCameraRay) < 0.0f)
            //{
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
            facette clipped[2];
            nClippedTriangles = clipped[0].ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

            // We may end up with multiple triangles from the clip, so project as
            // required
            for (int n = 0; n < nClippedTriangles; n++)
            {
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
                triProjected.p[0].x *= 600.0f;
                triProjected.p[0].y *= 600.0f;
                triProjected.p[1].x *= 600.0f;
                triProjected.p[1].y *= 600.0f;
                triProjected.p[2].x *= 600.0f;
                triProjected.p[2].y *= 600.0f;

                // Store triangle for sorting
                vecTrianglesToRaster.push_back(triProjected);
            }
            //}
        }

        // Sort triangles from back to front
        sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](facette& t1, facette& t2)
             {
                 float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
                 float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
                 return z1 > z2;
             });

        // Loop through all transformed, viewed, projected, and sorted triangles
        for (auto& triToRaster : vecTrianglesToRaster)
        {
            // Clip triangles against all four screen edges, this could yield
            // a bunch of triangles, so create a queue that we traverse to
            //  ensure we only test new triangles generated against planes
            facette clipped[2];
            std::list<facette> listTriangles;

            // Add initial triangle
            listTriangles.push_back(triToRaster);
            size_t nNewTriangles = 1;

            for (int p = 0; p < 4; p++)
            {
                int nTrisToAdd = 0;
                while (nNewTriangles > 0)
                {
                    // Take triangle from front of queue
                    facette test = listTriangles.front();
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
                    case 0:	nTrisToAdd = clipped[0].ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                    case 1:	nTrisToAdd = clipped[0].ClipAgainstPlane({ 0.0f, 1199.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                    case 2:	nTrisToAdd = clipped[0].ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                    case 3:	nTrisToAdd = clipped[0].ClipAgainstPlane({ 1199.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
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
            for (auto& t : listTriangles)
            {
                QPolygonF poly;
                poly << t.p[0].toPointF() << t.p[1].toPointF() << t.p[2].toPointF();

                //TriangleItem *ti = new TriangleItem(parent, poly, t.id, t.col);
                TriangleItem *ti = new TriangleItem(pool[t.col].couleur, poly, t.id, t.col);
                basculeSelectionChanged(false);
                for(int s: sauveSel) {
                    if (s == t.id) {
                        ti->setSelected(true);
                        break;
                    }
                }
                basculeSelectionChanged(true);
                scene3d->addItem(ti);
            }
            scene3d->update();
        }
    }
}

QGraphicsRectItem* Depliage::ajoutePage()
{
    QGraphicsRectItem * page;
    QPen pPage;

    int p = static_cast<int>(pages.size());
    page = new QGraphicsRectItem(p*220, 0, 210 ,297);

    QGraphicsPolygonItem * marge = new QGraphicsPolygonItem();
    marge->setPen(QPen(QBrush(Qt::red), 3));
    marge->setZValue(-1);
    QPolygonF pMarge;

    qreal a = 9;
    qreal b = 24;
    qreal c = 118;
    qreal d = 205;

    pMarge << QPoint(a + b, a + b + d + b + a)
           << QPoint(a + b, a + b + d + b)
           << QPoint(a, a + b + d + b)
           << QPoint(a, a + b + d)
           << QPointF(0, a + b + d)
           << QPointF(0, a + b)
           << QPointF(a, a + b)
           << QPointF(a, a)
           << QPointF(a + b, a)
           << QPointF(a + b, 0)
           << QPointF(a + b + c, 0)
           << QPointF(a + b + c, a)
           << QPointF(a + b + c + b, a)
           << QPointF(a + b + c + b, a + b)
           << QPointF(a + b + c + b + a, a + b)
           << QPointF(a + b + c + b + a, a + b + d)
           << QPointF(a + b + c + b, a + b + d)
           << QPointF(a + b + c + b, a + b + d + b)
           << QPointF(a + b + c, a + b + d + b)
           << QPointF(a + b + c, a + b + d + b + a);
    marge->setPolygon(pMarge);
    marge->setParentItem(page);
    QPointF delta = {
        page->rect().width() - (a+b+c+b+a),
        page->rect().height() -(a+b+d+b+a) };
    delta /= 2;
    marge->setPos(delta+ QPoint(220 *p, 0));

    page->setData(0, QVariant(p));
    pPage = QPen(Qt::blue);
    pPage.setWidth(3);
    page->setPen(pPage);
    //page->setFlag(QGraphicsItem::ItemIsMovable);
    pages.push_back(page);
    scene2d->addItem(page);

    return page;
}

void Depliage::creeFaces2d()
{   // liste 2d basique
    qreal deltaW = 2;
    qreal deltaH = 2;
    scene2d->clear();

    qreal H = 2;
    qreal W = 0;

    QGraphicsRectItem * pageCourante = ajoutePage();
    TriangleItem * gp;
    for (auto&& t : meshModel->faces) {
        Triangle2d t2 = t.d2ize();
        QPolygonF p;
        p << t2.a*50 << t2.b*50 << t2.c*50;
        p.translate(-p.boundingRect().left(), -p.boundingRect().top());
        W = p.boundingRect().width();
        H = std::max(H, p.boundingRect().height());
        bool ok = false;
        if ( ((W + deltaW) < (pageCourante->boundingRect().right() -2))
          && ((H + deltaH) < (pageCourante->boundingRect().bottom() -2)) )
        {
            p.translate(deltaW, deltaH);
            ok = true;
        } else {
            if ((H + deltaH) < (pageCourante->boundingRect().bottom() -2))
            {   // descend dans la page
                deltaW = pageCourante->boundingRect().left() +2;
                deltaH += H;
                if ((H + deltaH) < (pageCourante->boundingRect().bottom() -2))
                {
                    p.translate(deltaW, deltaH);
                    H = p.boundingRect().height();
                    ok = true;
                }
            }
        }

        if (!ok)
        {   // nouvelle page
            pageCourante = ajoutePage();

            // debut sur nouvelle page
            deltaH = 2;
            deltaW = pageCourante->boundingRect().left() +2;
            p.translate(deltaW, deltaH);
            H = p.boundingRect().height();
        }
        gp = new TriangleItem(this->pool[t.col].couleur , p, t.id, t.col);
        gp->setZValue(2);
        t2d.push_back(gp);
        //gp->setFlag(QGraphicsItem::ItemIsMovable);
        scene2d->addItem(gp);
        deltaW = p.boundingRect().right() + 2;
    }
}

void Depliage::trouveVoisinage()
{
    int i;
    int vi;
    bool ok;
    int nbFaces = meshModel->faces.size();
    std::array<Voisin, 3> tmpV;

    for (auto &&ti : meshModel->faces) {
        i = ti.id;
        for (int j = 0; j < 3; j++) {
            vi = 0;
            ok = false;
            do {
                if (vi != i) {
                    for (int k = 0; (k < 3) && !ok; k++)
                        if ( (meshModel->faces[vi].pi[k] == ti.pi[next(j)])
                            && (meshModel->faces[vi].pi[next(k)] == ti.pi[j]) ) {

                            // calcule coplanéité
                            int p;
                            if (meshModel->faces[vi].eq3(ti, 0))
                                p = 0;
                            else if(meshModel->faces[vi].eq3(ti, 1))
                                p = 1;
                            else
                                p = 2;

                            qreal c = meshModel->faces[vi].isCoplanar(ti.p[p]);
                            tmpV[j] = Voisin(j, i, vi, next(k), sgn(c));
                            //tmpV[j] = Voisin(i, vi, next(k), sgn(c));
                            if (j == 2) {
                                ti.voisins = tmpV;
                                //qDebug() << i << ti.pi[0] << ti.pi[1] << ti.pi[2] << ti.voisins[0].nF << ti.voisins[1].nF << ti.voisins[2].nF;
                            }
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

void Depliage::basculeSelectionChanged(bool etat) {
    if (etat) {
        parent->connect(scene3d, &QGraphicsScene::selectionChanged, parent, &MainWindow::SelectionDansScene3D);
        parent->connect(scene2d, &QGraphicsScene::selectionChanged, parent, &MainWindow::SelectionDansScene2D);
    } else {
        parent->disconnect(scene3d, &QGraphicsScene::selectionChanged, parent, &MainWindow::SelectionDansScene3D);
        parent->disconnect(scene2d, &QGraphicsScene::selectionChanged, parent, &MainWindow::SelectionDansScene2D);
    }
}

// next et prev retournent le prochain/précédent dans
// une liste contenant [0,1,2] en bouclant si besoin
int next(int n) { return n == 2 ? 0 : n+1; }
int prev(int n) { return n == 0 ? 2 : n-1; }
