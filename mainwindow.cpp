// Interface de l'application Deplieur
//---------------------------------------------------------
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "triangleitem3d.h"
#include "triangleitem2d.h"
#include "piecepolygonitem.h"
#include "pieceligneitem.h"
#include "piecelangitem.h"
#include "piecenumitem.h"
#include "svg.hpp"

#include <QToolBar>
#include <QToolButton>
#include <QMenu>
#include <QTableWidgetItem>
#include <QColorDialog>
#include <QFileDialog>

#include <QDebug>
//---------------------------------------------------------
void bascule (QObject *o) {
    // Affiche/Cache un widget
    QWidget *w = static_cast<QWidget*>(o);
    if (w)
        w->setVisible(!w->isVisible());
}
//---------------------------------------------------------
QLabel* creeColorLabel (QColor couleur) {
    QPixmap pixmap(15, 15);
    pixmap.fill(couleur);
    QLabel *label = new QLabel();
    label->setPixmap(pixmap);
    label->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    label->setTextInteractionFlags(Qt::NoTextInteraction);
    label->setFocusPolicy(Qt::NoFocus);

    return label;
}
//---------------------------------------------------------
// DESTRUCTEUR
MainWindow::~MainWindow () {
    // LIBERE LA MEMOIRE
    delete ui;
}

void MainWindow::resizeEvent (QResizeEvent* event) {
    ajuste3D();
    QMainWindow::resizeEvent(event);
}

void MainWindow::clicPli () {
    if (scene2d->selectedItems().isEmpty())
        return;

    QGraphicsItem *gi = scene2d->selectedItems().first();

    int id1 = gi->data(0).toInt();
    int id2 = gi->data(1).toInt();
}

void MainWindow::changeEchelle()
{
    bool ok;
    qreal n = leEchelle->text().toFloat(&ok);

    if (ok) {
        dep.echelle = n;
        for (auto && i : scene2d->items()) {
            TriangleItem2d *t = qgraphicsitem_cast<TriangleItem2d*>(i);
            if (t) {
                QTransform transform;
                transform.scale(n, n);
                t->setPolygon(transform.map(t->polygon()));
            }
        }
        piecesMAJ();
    }
    vec3d d = dep.dim.Vector_Mul(50*n);
    ui->statusbar->showMessage(QString("Dim : %1 %2 %3").arg(d.x, 0, 'f', 0).arg(d.y, 0, 'f', 0).arg(d.z, 0, 'f', 0));
}

void MainWindow::changeMarge (int index) {
    scene2d->margeId = index;
    scene2d->update();
}

void MainWindow::changeTypeLang (int typeLang) {
    dep.typeLang = typeLang;
    for (auto && p : dep.pieces) {
        for (auto && l : p.lignes) {
            Nums n = Nums(l.id1, l.id2);
            Nums *nf = &(dep.nums[dep.nums.indexOf(n)]);
            switch (typeLang) {
            case 0 :
                nf->tlang = L00;
                l.bLang = false;
                break;
            case 1 :
                nf->tlang = L10;
                l.bLang = nf->id1 == l.id1;
                break;
            case 2 :
                nf->tlang = L11;
                l.bLang = true;
            }
        }
    }
    piecesMAJ();
}

void MainWindow::basculeLanguette(int id1, int id2) {
    Nums n = Nums(id1, id2);
    Nums *nf = &(dep.nums[dep.nums.indexOf(n)]);

    nf->tlang = (nf->tlang == L01) ? L10 : L01;
    piecesMAJ();
}

void MainWindow::hoverOn(int faceId)
{
    for (auto && f : dep.faces){
        if (f.triangleItem->hoverOn){
            f.triangleItem->hoverOn = false;
            f.triangleItem->setVisible(false);
        }
    }
    dep.faces[faceId].triangleItem->hoverOn = true;
    dep.faces[faceId].triangleItem->setVisible(true);
}

void MainWindow::hoverOff(int faceId)
{
    dep.faces[faceId].triangleItem->hoverOn = false;
    dep.faces[faceId].triangleItem->setVisible(false);
}

void MainWindow::changeCouleur (int couleur) {
    ui->tableCouleurs->selectRow(couleur);
    couleurClic(couleur, 0);
}

void MainWindow::changeNBCouleur (int ligne, int delta) {
    Piece *piece = &(dep.pieces[ligne]);
    piece->nb = piece->nb + delta;
    QTableWidgetItem *twi = new QTableWidgetItem(QString::number(piece->nb));
    ui->tableCouleurs->setItem(ligne, 2, twi);
}

void MainWindow::changeFaceCouleur (int faceId, int couleurId) {
    dep.faces[faceId].col = couleurId;
}

void MainWindow::pieceEnleveFace (int pieceId, int faceId) {
    Piece *piece = &(dep.pieces[pieceId]);
    Facette *facette = &(dep.faces[faceId]);
    TriangleItem2d *tCible = facette->triangleItem;
    QColor blanc = QColor(Qt::white);
    bool ok = true;
    for (auto && e : piece->elements2) {
        if (e.de == faceId) {
            ok = false;
            break;
        }
    }

    if (!ok) {
        ui->statusbar->showMessage("Impossible !");
        return;
    }

    ok = false;
    if (piece->elements.removeOne(faceId)) {
        Attache att(faceId);
        piece->elements2.removeOne(att);
        tCible->setParentItem(0);
        if (tCible->hoverOn)
            hoverOff(tCible->id);
        QPointF delta = piece->bord->pos();
        tCible->moveBy(-delta.x(), -delta.y());
        tCible->setPos(0,0);

        changeNBCouleur(pieceId, -1);
        changeNBCouleur(0, +1);

        if (piece->elements.isEmpty()) {
            scene2d->removeItem(piece->bord);
            delete piece->bord;
            piece->bord = nullptr;
            pieceId = 0;
            tCible->setPos(0,0);
        }

        facette->col = 0;
        tCible->col = 0;
        tCible->pieceCouleur = blanc;

        piecesMAJ();
        face3dMAJ(&(dep.pieces[pieceId]), faceId);

        for (auto&& f3d : scene3d->items()) {
            TriangleItem3d *fI = qgraphicsitem_cast<TriangleItem3d*>(f3d);
            if (fI->id == faceId) {
                fI->col = 0;
                fI->pieceCouleur = blanc;
                fI->update();
            }
        }
        ui->statusbar->showMessage("Face enlevée !");
        scene3d->update();
    }
}

void MainWindow::pieceCreeLignes (Piece *piece) {
    piece->lignes.clear();

    QPointF delta = dep.faces[piece->elements.first()].triangleItem->pos();
    QTransform transform;
    transform.translate(delta.x(), delta.y());
    Nums n1, nF;
    int nC;
    for (auto && n : piece->elements) {
        Facette *face = &(dep.faces[n]);
        QPolygonF P = transform.map(face->triangleItem->polygon());

        for (int i = 0; i < 3; i++) {
            QPointF P1 = P[i];
            QPointF P2 = P[next(i)];
            Voisin V = face->voisins[i];
            int nl = -1;
            int il = 0;
            for (auto&& R : piece->lignes) {
                if ((eq(R.p1, P1) && eq(R.p2, P2))
                    || (eq(R.p1, P2) && eq(R.p2, P1))) {
                    nl = il;
                    break;
                } else
                    il++;
            }
            if (nl > -1) {
                piece->lignes[nl].nb++;
            } else {
                Ligne ligne (P1, P2, V.nF, V.pnF, V.cop);
                switch (cbLanguettes->currentIndex()) {
                case 0 :
                    ligne.bLang = false;
                    break;
                case 1 :
                    nC = dep.chercheNum(ligne.id1, ligne.id2);
                    nF = dep.nums[dep.nums.indexOf(Nums(ligne.id1, ligne.id2))];
                    if (nF.tlang == L01)
                        ligne.bLang = (ligne.id1 == nF.id2) && (ligne.id2 == nF.id1);
                    else if (nF.tlang == L10)
                        ligne.bLang = (ligne.id1 == nF.id1) && (ligne.id2 == nF.id2);
                    break;
                case 2 :
                    ligne.bLang = true;
                }

                piece->lignes.append(ligne);
            }
        }
    }
}

void MainWindow::pieceAjouteFace (int pieceId, int faceId) {
    Piece *piece = &(dep.pieces[pieceId]);
    Facette *facette = &(dep.faces[faceId]);
    TriangleItem2d *tCible = facette->triangleItem;
    TriangleItem2d *tSource;
    bool ok = false;

    if (piece->nb == 0) { // piece vide -> créer groupe + ajouter 1ère face
        if (!piece->bord) {
            piece->bord = new PiecePolygonItem(scene2d, piece->couleur);
            //if (dep.echelle != 1)
            //    piece->bord->setScale(dep.echelle);
        }

        tCible->setParentItem(piece->bord);
        changeNBCouleur(facette->col, -1);
        changeNBCouleur(pieceId, +1);
        facette->col = pieceId;
        QPointF centre = scene2d->pageTemoin->boundingRect().center();
        piece->bord->setPos( centre -
             QPointF(tCible->boundingRect().width()/2,
                     tCible->boundingRect().height()/2));
        tCible->col = pieceId;
        tCible->pieceCouleur = piece->couleur;
        piece->elements.append(faceId);
        Attache att(-1, faceId);
        piece->elements2.append(att);
        ok = true;
    } else {
        // piece non vide -> chercher voisin
        // -> si trouvé : lier face avec voisin + ajouter face
        bool voisinTrouve = false;
        Voisin *vT;
        if (scene3d->dernFace > -1) {
            auto&& i = dep.faces[scene3d->dernFace];
            if (i.col == ui->tableCouleurs->currentRow()) {
                for (auto && v : dep.faces[scene3d->dernFace].voisins) {
                    if (v.nF == faceId) {
                        voisinTrouve = true;
                        vT = &v;
                        break;
                    }
                }
            }
        }
        if (voisinTrouve) {
            ui->statusbar->showMessage("Ok");
            tSource = dep.faces[vT->pnF].triangleItem;

            QPolygonF pCible = tCible->polygon();
            QPolygonF pSource = tSource->polygon();
            QPointF ptOrig = pSource[vT->id];
            QPointF delta = ptOrig - pCible[vT->idx];
            QTransform trT;
            trT.translate(delta.x(), delta.y());
            pCible = trT.map(pCible);

            QTransform trR;
            qreal angle = calc_angle(
                ptOrig,
                pSource[next(vT->id)],
                pCible[prev(vT->idx)]);
            trR.translate(ptOrig.x(), ptOrig.y());
            trR.rotate(-angle);
            trR.translate(-ptOrig.x(), -ptOrig.y());

            tCible->setPolygon(trR.map(pCible));
            tCible->setParentItem(piece->bord);

            changeNBCouleur(facette->col, -1);
            changeNBCouleur(pieceId, +1);
            facette->col = pieceId;
            tCible->col = pieceId;
            tCible->pieceCouleur = piece->couleur;
            piece->elements.append(faceId);
            Attache att(scene3d->dernFace, faceId);
            piece->elements2.append(att);
            ok = true;
            ui->statusbar->showMessage("Face ajoutée !");
        }
        else {
            ui->statusbar->showMessage("voisin non trouvé");
        }
    }    
    if (ok) {
        piecesMAJ();
        face3dMAJ(piece, faceId);
    }
}

void MainWindow::piecesMAJ () {
    for (auto&& n : dep.nums) { // supprime les numéros
        n.num = -1;
    }
    dep.prochainNum = 0;

    // pas de piece "0"
    for (int i = 1; i < dep.pieces.size(); i++) {
        pieceMAJ(&(dep.pieces[i]));
    }
}

void MainWindow::pieceMAJ (Piece *piece) {
    if (piece->elements.isEmpty())
        return;

    pieceCreeLignes(piece);

    for (auto&& i : piece->bord->childItems()) {
        PieceLigneItem *gli = qgraphicsitem_cast<PieceLigneItem*>(i);
        if (gli) {
            gli->setParentItem(0);
            scene2d->removeItem(gli);
            delete gli;
        }
    }

    // recalc bLang
    for (auto&& l : piece->lignes) {
        int num = (l.nb == 1) ? dep.chercheNum(l.id1, l.id2) : -1;
        PieceLigneItem *gli = new PieceLigneItem(piece->bord, &l, num);
    }
    piece->pieceConstruitBord();
}

void MainWindow::face3dMAJ (Piece *piece, int faceId) {
    for (auto&& f3d : scene3d->items()) {
        TriangleItem3d *fI = qgraphicsitem_cast<TriangleItem3d*>(f3d);
        if (fI->id == faceId) {
            fI->col = piece->id;
            fI->pieceCouleur = piece->couleur;
            fI->update();
        }
    }
}

void MainWindow::peutColorierFace (int faceId) {
    // CLIC SUR UNE FACE 3D : COLORIER OU NON ?
    Facette *facette = &(dep.faces[faceId]);
    int coul = ui->tableCouleurs->currentRow();

    qDebug() << scene3d->dernFace << scene3d->faceCourante;

    // SI COUL = 0
    // -> SI COUL FACE = 0 -> ne rien faire
    // ----> SINON ENLEVER FACE DE SA PIECE ACTUELLE
    if (coul == 0) {
        if (facette->col == 0) {
            //qDebug() << "RIEN A FAIRE";
        } else {
            //qDebug() << "ENLEVER DE PIECE (coul = " << facette->col << ")";
            pieceEnleveFace(facette->col, facette->id);
        }
    } else {
        if (facette->col == 0) {
            //qDebug() << "AJOUTER A PIECE (coul = " << coul << ")";
            pieceAjouteFace(coul, facette->id);
        } else {
            //qDebug() << "ENLEVER DE PIECE (coul = " << facette->col << ")";
            pieceEnleveFace(facette->col, facette->id);
            //qDebug() << "AJOUTER A PIECE (coul = " << coul << ")";
            pieceAjouteFace(coul, facette->id);
        }
    }
}

void MainWindow::exporte () {
    SVG::SVG root;
    QByteArray svgRoot;

    qreal s = 5.0f / 1.76f;
    //QTransform tSPage;
    //tSPage.scale(s, s);
    //s *= dep.echelle;
    QTransform tS;
    tS.scale(s, s);

    root.set_attr("width", QString("%1mm").arg(dep.dimPage.x()).toStdString());
    root.set_attr("height", QString("%1mm").arg(dep.dimPage.y()).toStdString());
    QPoint scaledPageDim = tS.map(dep.dimPage);
    root.set_attr("viewBox", QString("0 0 %1 %2").arg(scaledPageDim.x()).arg(scaledPageDim.y()).toStdString());

    // Basic CSS support
    root.style("path").set_attr("fill", "none");


    SVG::Group *gPage, *gNums;
    SVG::Path *chCoupe, *chPliM, *chPliV;

    QList <SVG::Path *> LCoupes;
    QList <SVG::Path *> LPlisM;
    QList <SVG::Path *> LPlisV;
    QList <SVG::Group *> LNums;

    piecesMAJ();

    // recherche nb pages
    int maxP = 0;
    for (auto && p : dep.pieces) {
        if (!p.bord)
            continue;
        int x = p.bord->x() / (dep.dimPage.x()+10);
        if (x > maxP)
            maxP = x;
    }

    for (int i = 0; i <= maxP; i++) {
        gPage = root.add_child<SVG::Group>();
        QString snum = QString("_%1").arg(i+1, 3, 10, QChar('0'));
        gPage->set_attr("id", QString("page%1").arg(snum).toStdString());

        chCoupe = gPage->add_child<SVG::Path>();
        LCoupes.append(chCoupe);
        chCoupe->set_attr("id", QString("coupes%1").arg(snum).toStdString());
        chCoupe->set_attr("stroke", "red");
        chCoupe->set_attr("stroke-width", "1");
        chCoupe->set_attr("fill", "beige");

        chPliM = gPage->add_child<SVG::Path>();
        LPlisM.append(chPliM);
        chPliM->set_attr("id", QString("plisM%1").arg(snum).toStdString());
        chPliM->set_attr("stroke", "brown");
        chPliM->set_attr("stroke-dasharray", "12 12");
        chPliM->set_attr("stroke-width", "1");

        chPliV = gPage->add_child<SVG::Path>();
        LPlisV.append(chPliV);
        chPliV->set_attr("id", QString("plisV%1").arg(snum).toStdString());
        chPliV->set_attr("stroke", "green");
        chPliV->set_attr("stroke-dasharray", "8 2 1 2");
        chPliV->set_attr("stroke-width", "1");

        gNums = gPage->add_child<SVG::Group>();
        LNums.append(gNums);
        gNums->set_attr("id", QString("nums%1").arg(snum).toStdString());
    }

    QPointF centre, b, p1, p2, delta;
    Nums num1;
    for (auto && n : dep.pieces) {
        if (!n.bord)
            continue;

        int x = (n.bord->x() / dep.dimPage.x());
        chCoupe = LCoupes[x];
        chPliM = LPlisM[x];
        chPliV = LPlisV[x];
        gNums = LNums[x];

        delta = n.bord->pos();
        for (auto && l : n.lignes) {
            p1 = l.p1 + delta;
            p2 = l.p2 + delta;
            p1 = tS.map(p1);
            p2 = tS.map(p2);
            QLineF ligne = QLineF(p1, p2);
            QPolygonF pLang;
            if (l.nb == 1) {
                if (l.bLang) {
                    pLang = lineToLang(p1, p2, true).toFillPolygon();
                    chCoupe->move_to(pLang[0].x(), pLang[0].y());
                    chCoupe->line_to(pLang[1].x(), pLang[1].y());
                    chCoupe->line_to(pLang[2].x(), pLang[2].y());
                    chCoupe->line_to(pLang[3].x(), pLang[3].y());
                    if (l.cop > 0) {
                        chPliV->move_to(p1.x(), p1.y());
                        chPliV->line_to(p2.x(), p2.y());
                    } else {
                        chPliM->move_to(p1.x(), p1.y());
                        chPliM->line_to(p2.x(), p2.y());
                    }
                } else {
                    chCoupe->move_to(p1.x(), p1.y());
                    chCoupe->line_to(p2.x(), p2.y());
                }

                num1 = Nums(l.id1, l.id2);
                QString chNumero = QString::number(dep.nums[dep.nums.indexOf(num1)].num);
                SVG::Text *numero;
                int dX;
                if (l.bLang && (ligne.length() > 20)) {
                    centre = centroid4(pLang);
                    dX = 0;  // 12
                } else {
                    dX = -7;
                    centre =  ligne.center();
                }
                numero = gNums->add_child<SVG::Text>(0, dX, chNumero.toStdString());
                int angle = 180 - ligne.angle();
                numero->set_attr("font-family", "monospace");
                numero->set_attr("font-size", "12pt");
                numero->set_attr("text-anchor", "middle");
                numero->set_attr("dominant-baseline", "central");
                numero->set_attr("transform", QString("translate (%1 %2) rotate(%3)")
                    .arg(centre.x()).arg(centre.y()).arg(angle).toStdString());
            } else {
                if (l.cop < 0) {
                    chPliM->move_to(p1.x(), p1.y());
                    chPliM->line_to(p2.x(), p2.y());
                } else if (l.cop > 0) {
                    chPliV->move_to(p1.x(), p1.y());
                    chPliV->line_to(p2.x(), p2.y());
                }
            }
        }
    }
    svgRoot = QByteArray::fromStdString(std::string(root));
    QFileDialog::saveFileContent(svgRoot, "myExport.svg");
}

QPainterPath MainWindow::construitChemin(QList<QLineF> lignes) {
    QPainterPath chemin;
    for (auto && l : lignes) {
        chemin.moveTo(l.p1().x(), l.p1().y());
        chemin.lineTo(l.p2().x(), l.p2().y());
    }
    return chemin;
}

void MainWindow::ajoutePage () {
    scene2d->nbPages++;
    scene2d->update();
    ajuste2D();
}

void MainWindow::supprimePage () {
    if (scene2d->nbPages > 1) {
        scene2d->nbPages--;
        scene2d->update();
        ajuste2D();
    }
}

void MainWindow::tourner3DXD () {
    if (!dep.ModeleOK)
        return;

    dep.fThetaX += dep.fPas;
    dep.dessineModele(scene3d);
}

void MainWindow::tourner3DXG () {
    if(!dep.ModeleOK)
        return;

    dep.fThetaX -= dep.fPas;
    dep.dessineModele(scene3d);
}

void MainWindow::tourner3DYD () {
    if(!dep.ModeleOK)
        return;

    dep.fThetaY += dep.fPas;
    dep.dessineModele(scene3d);
}

void MainWindow::tourner3DYG () {
    if(!dep.ModeleOK)
        return;

    dep.fThetaY -= dep.fPas;
    dep.dessineModele(scene3d);
}

void MainWindow::tourner3DZD () {
    if(!dep.ModeleOK)
        return;

    dep.fThetaZ += dep.fPas;
    dep.dessineModele(scene3d);
}

void MainWindow::tourner3DZG () {
    if(!dep.ModeleOK)
        return;

    dep.fThetaZ -= dep.fPas;
    dep.dessineModele(scene3d);
}

void MainWindow::tourneModele (qreal dZ, qreal dX) {
    dep.fThetaZ += dZ;
    dep.fThetaX -= dX;
    dep.dessineModele(scene3d);
}

void MainWindow::tourne2D (qreal a) {
    if (scene2d->selectedItems().isEmpty())
        return;

    PiecePolygonItem *bord = static_cast<PiecePolygonItem*>(scene2d->selectedItems().first());
    if (bord) {
        QPointF centre = bord->boundingRect().center();
        QTransform transform;
        transform.translate(centre.x(), centre.y()).rotate(a).translate(-centre.x(), -centre.y());
        bord->setPolygon(transform.map(bord->polygon()));

        for (auto&& i : bord->childItems()) {
            QGraphicsPolygonItem *p = qgraphicsitem_cast<QGraphicsPolygonItem*>(i);
            if (p) {
                p->setPolygon(transform.map(p->polygon()));
                continue;
            }

            TriangleItem2d *t = qgraphicsitem_cast<TriangleItem2d*>(i);
            if (t) {
                t->setPolygon(transform.map(t->polygon()));
                continue;
            }

            PieceLigneItem *l = qgraphicsitem_cast<PieceLigneItem*>(i);
            if (l) {
                l->setLine(transform.map(l->line()));
                for (auto && lc : l->childItems()) {
                    PieceLangItem *pla = qgraphicsitem_cast<PieceLangItem*>(lc);
                    if (pla) {
                        pla->setPath(transform.map(pla->path()));
                        continue;
                    }
                    PieceNumItem *lt = qgraphicsitem_cast<PieceNumItem*>(lc);
                    if (lt) {
                        QPointF b = QPointF(lt->boundingRect().width() / 2,
                                            lt->boundingRect().height() + dep.dYt);
                        lt->setTransformOriginPoint(b);
                        lt->setRotation(180- l->line().angle());
                        QPointF centre = l->line().center() - b;
                        lt->setPos(centre);
                        //continue;
                    }
                }
            }
        }
        bord->update();
    }
}

void MainWindow::ajuste3D () {
    ui->vue3d->fitInView(ui->vue3d->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void MainWindow::ajuste2D () {
    if (!scene2d->pageTemoin) {
        scene2d->pageTemoin = new QGraphicsRectItem( 0, 0, 220* scene2d->nbPages, 297);
        scene2d->pageTemoin->setPen(QPen(Qt::NoPen));
        scene2d->pageTemoin->setData(0, QVariant(1));
        scene2d->addItem(scene2d->pageTemoin);
    } else
        scene2d->pageTemoin->setRect(0, 0, 220* scene2d->nbPages, 297);
    ui->vue2d->fitInView(ui->vue2d->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void MainWindow::couleurClic (int ligne, int col) {
    // clic sur un item du tableau des couleurs
    if (col > 0)
        return;

    scene3d->itemColorId = ligne;
    scene3d->itemColor = dep.pieces[ligne].couleur;
}

void MainWindow::bascule2d () {
    // AFFICHE/CACHE LA VUE 2D
    bascule(ui->splitter->children()[0]);
    ajuste3D();
    ajuste2D();
}

void MainWindow::bascule3d () {
    // AFFICHE/CACHE LA VUE 3D
    bascule(ui->splitter->children()[1]);
    ajuste3D();
    ajuste2D();
}

void MainWindow::basculeCouleurs () {
    // AFFICHE/CACHE LES COULEURS
    bascule(ui->splitter->children()[2]);
    ajuste3D();
    ajuste2D();
}

void MainWindow::couleurChoisie (QColor color) {
    // Une couleur a été choisie ...
    if (!dep.ModeleOK)
        return;

    if (!color.isValid())
        return;

    bool ok = true;
    for (auto && pc :dep.pieces) {
        if (pc.couleur == color) {
            ok = false;
            break;
        }
    }
    if (ok) {
        Piece p1;
        p1.couleur = color;
        p1.nb = 0;
        int nb = ui->tableCouleurs->rowCount();
        ui->tableCouleurs->setRowCount(nb+1);
        p1.id = nb;
        dep.pieces.append(p1);
        ui->tableCouleurs->setCellWidget(nb, 0, creeColorLabel(color));
        ui->tableCouleurs->setCellWidget(nb, 1, creeColorLabel(color));
        ui->tableCouleurs->setItem(nb, 2, new QTableWidgetItem("0"));
        QString s = QString("Piece #%1").arg(nb);
        ui->tableCouleurs->setItem(nb, 3, new QTableWidgetItem(s));
        ui->statusbar->showMessage("Couleur ajoutée");
        ui->tableCouleurs->selectRow(nb);
        couleurClic(nb, 0);
    } else {
        ui->statusbar->showMessage("Couleur déjà utilisée !");
    }
}

void MainWindow::couleurNouveau () {
    // Le bouton "+" (couleurs) a été appuyé ...
    if(!dep.ModeleOK)
        return;

    QColorDialog *dialog = new QColorDialog();
    dialog->setOption(QColorDialog::NoEyeDropperButton);
    connect(dialog, &QColorDialog::colorSelected, this, [this](const QColor& color) {couleurChoisie(color);});
    dialog->open();
}

void MainWindow::demo() {
    dep.nums.clear();
    dep.pieces.clear();
    dep.faces.clear();
    dep = Depliage();
    dep.ModeleOK = true;
    dep.echelle = 1;
    dep.fPas = 0.1;
    scene2d->nbPages = 1;
    leEchelle->setText(QString::number(dep.echelle, 'g', 2));
    setWindowTitle("Deplieur [Modele Demo]");
    demoMode = true;
    dep.chargeFichierOBJ(m_demoFichier->downloadedData());
    chargeFichier();
}

void MainWindow::nouveau () {
    // Nouveau projet : choisit un fichier .obj et le charge
    auto fileContentReady = [this](const QString &fileName, const QByteArray &fileContent) {
        if (!fileName.isEmpty()) {
            // Use fileName and fileContent
            dep.nums.clear();
            dep.pieces.clear();
            dep.faces.clear();
            dep = Depliage();
            dep.ModeleOK = true;
            scene3d->itemColorId = -1;
            scene3d->itemColor = Qt::white;
            delete(scene2d->pageTemoin);
            leEchelle->setText(QString::number(dep.echelle, 'g', 2));
            QFileInfo f(fileName);
            setWindowTitle("Deplieur [" + f.fileName() + "]");
            dep.chargeFichierOBJ(fileContent);
            chargeFichier();
        }
    };
    QFileDialog::getOpenFileContent("fichier OBJ (*.obj)",  fileContentReady);
}

void MainWindow::chargeFichier() {
    Piece p0;
    p0.id = 0;
    p0.couleur = Qt::white;
    p0.nb = dep.faces.size();
    dep.pieces.append(p0);

    changeNBCouleur(0);
    ui->tableCouleurs->setRowCount(1);
    dep.dessineModele(scene3d);
    connect(scene3d, &DepliageScene::changeCouleur, this, &MainWindow::changeCouleur);
    connect(scene3d, &DepliageScene::changeNBCouleur, this, &MainWindow::changeNBCouleur);
    connect(scene3d, &DepliageScene::changeFaceCouleur, this, &MainWindow::changeFaceCouleur);
    connect(ui->vue3d, &DepliageVue3d::tourneModele, this, &MainWindow::tourneModele);
    connect(ui->vue2d, &DepliageVue2d::tourne2D, this, &MainWindow::tourne2D);
    connect(scene3d, &DepliageScene::peutColorierFace, this, &MainWindow::peutColorierFace);
    connect(scene3d, &DepliageScene::pieceEnleveFace, this, &MainWindow::pieceEnleveFace);
    connect(scene2d, &DepliageScene::basculeLanguette, this, &MainWindow::basculeLanguette);
    connect(scene3d, &DepliageScene::hoverOn, this, &MainWindow::hoverOn);
    connect(scene3d, &DepliageScene::hoverOff, this, &MainWindow::hoverOff);

    //connect(scene2d, &DepliageScene::selectionChanged, this, &MainWindow::clicPli);

    scene2d->nbPages = 1;
    dep.creeFaces2d(scene2d);
    dep.trouveVoisinage();

    //connect(dep->scene2d, &DeplieurScene::changeCouleur, this, &MainWindow::changeCouleur);
    //dep.basculeSelectionChanged(true);

    vec3d d = dep.dim.Vector_Mul(50);
    ui->statusbar->showMessage(QString("Dim : %1 %2 %3").arg(d.x, 0, 'f', 0).arg(d.y, 0, 'f', 0).arg(d.z, 0, 'f', 0));

    ajuste3D();
    ajuste2D();
}

void MainWindow::lanceDemo(int index) {
    if (index == 0 )
        return;

    QString chUrl = cbDemo->itemData(index).toString();
    //QUrl demoUrl(QString("https://github.com/gilboonet/gilboonet.github.io/raw/refs/heads/master/modeles/%1.obj").arg(chUrl));
    QUrl demoUrl(QString("modeles/%1.obj").arg(chUrl));
    m_demoFichier = new FileDownloader(demoUrl, this);
    connect(m_demoFichier, &FileDownloader::downloaded, this, &MainWindow::demo);
}

// CONSTRUCTEUR
MainWindow::MainWindow (QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    // DEFINIT L'INTERFACE
    ui->setupUi(this);
    setWindowFlag(Qt::Window);

    dep.ModeleOK = false;

    // Menu principal
    QToolBar *tbMain = new QToolBar(this);
    tbMain->addAction(ui->actionNouveau);
    tbMain->addAction(ui->actionOuvrir);
    tbMain->addAction(ui->actionSauver);
    tbMain->addAction(ui->actionExporter);
    tbMain->addAction(ui->actionQuitter);

//    tbMain->addAction(ui->actionDemo);
//    connect(ui->actionDemo, &QAction::triggered, this, &MainWindow::lanceDemo1);

    tbMain->addSeparator();
    tbMain->addAction(ui->actionBasculeCouleurs);
    connect(ui->actionBasculeCouleurs, &QAction::triggered, this, &MainWindow::basculeCouleurs);
    tbMain->addAction(ui->actionBascule3D);
    connect(ui->actionBascule3D, &QAction::triggered, this, &MainWindow::bascule3d);
    tbMain->addAction(ui->actionBascule2D);
    connect(ui->actionBascule2D, &QAction::triggered, this, &MainWindow::bascule2d);

    tbMain->setOrientation(Qt::Vertical);
    tbMain->setMaximumWidth(25);
    ui->verticalLayoutMenu->addWidget(tbMain);

    connect(ui->actionNouveau, &QAction::triggered, this, &MainWindow::nouveau);
    connect(ui->actionQuitter, &QAction::triggered, this, &QApplication::quit);
    connect(ui->actionExporter, &QAction::triggered, this, &MainWindow::exporte);

    // Menu Pieces/couleurs
    QToolBar *tbCol = new QToolBar(this);
    tbCol->addAction(ui->actionCouleurPlus);
    connect(ui->actionCouleurPlus, &QAction::triggered, this, &MainWindow::couleurNouveau);

    tbCol->addAction(ui->actionCouleurMoins);
    //connect(ui->actionCouleurMoins, &QAction::triggered, this, &MainWindow::couleurSupprime);

    ui->verticalLayoutCouleurs->setMenuBar(tbCol);

    ui->tableCouleurs->setRowCount(1);
    ui->tableCouleurs->setColumnCount(4);
    ui->tableCouleurs->setColumnWidth(0, 5);
    ui->tableCouleurs->setColumnWidth(1, 5);
    ui->tableCouleurs->setColumnWidth(2, 35);
    ui->tableCouleurs->setColumnWidth(3, 108);
    // Première ligne : couleur white par défaut
    ui->tableCouleurs->setCellWidget(0, 0, creeColorLabel(Qt::white));
    ui->tableCouleurs->setCellWidget(0, 1, creeColorLabel(Qt::white));
    ui->tableCouleurs->setItem(0, 2, new QTableWidgetItem("0"));
    ui->tableCouleurs->setItem(0, 3, new QTableWidgetItem("Defaut"));
    connect(ui->tableCouleurs, &QTableWidget::cellPressed, this, &MainWindow::couleurClic);
    ui->tableCouleurs->clearSelection();


    // Menu 3d
    QToolBar *tb3d = new QToolBar(this);

    cbDemo = new QComboBox();
    cbDemo->addItem("Choisir demo", QVariant(""));
    cbDemo->addItem("Anubis Tête 102", QVariant("teteAnubisH20_2C"));
    cbDemo->addItem("Chat 234", QVariant("chat234"));
    cbDemo->addItem("Chat 310", QVariant("chat310"));
    cbDemo->addItem("Cheval 430", QVariant("cheval430"));
    cbDemo->addItem("Cheval Buste 120", QVariant("buste_cheval120"));
    cbDemo->addItem("Chien 354", QVariant("chien354"));
    cbDemo->addItem("Eléphant Assis 206", QVariant("elephantAssis206"));
    cbDemo->addItem("Faucon Maltais 300", QVariant("fauconM300"));
    cbDemo->addItem("Girafe 464", QVariant("girafe464"));
    cbDemo->addItem("Gorille 500", QVariant("gorille500"));
    cbDemo->addItem("Lapin Assis 192", QVariant("lapinAssis192"));
    cbDemo->addItem("Lapin Boston 146", QVariant("bunny146"));
    cbDemo->addItem("Lion Tête 358", QVariant("teteLion358"));
    cbDemo->addItem("Main 200", QVariant("main200"));
    cbDemo->addItem("Moaï 156", QVariant("moai156"));
    cbDemo->addItem("Moaï 312", QVariant("moai312"));
    cbDemo->addItem("Moineau 150", QVariant("moineau150"));
    cbDemo->addItem("Oeuf 220", QVariant("oeuf220"));
    cbDemo->addItem("Panthère 500", QVariant("panthere500"));
    cbDemo->addItem("Tdy 236", QVariant("tdy236"));
    cbDemo->addItem("Taureau 278", QVariant("taureau278"));
    cbDemo->addItem("Seth Buste254", QVariant("seth254"));

    cbDemo->setToolTip("Demos");
    tb3d->addWidget(cbDemo);
    connect(cbDemo, &QComboBox::currentIndexChanged, this, &MainWindow::lanceDemo);

    tb3d->addAction(ui->actionXG);
    connect(ui->actionXG, &QAction::triggered, this, &MainWindow::tourner3DXG);
    tb3d->addWidget(new QLabel("X"));
    tb3d->addAction(ui->actionXD);
    connect(ui->actionXD, &QAction::triggered, this, &MainWindow::tourner3DXD);

    tb3d->addSeparator();
    tb3d->addAction(ui->actionYG);
    connect(ui->actionYG, &QAction::triggered, this, &MainWindow::tourner3DYG);
    tb3d->addWidget(new QLabel("Y"));
    tb3d->addAction(ui->actionYD);
    connect(ui->actionYD, &QAction::triggered, this, &MainWindow::tourner3DYD);

    tb3d->addSeparator();
    tb3d->addAction(ui->actionZG);
    connect(ui->actionZG, &QAction::triggered, this, &MainWindow::tourner3DZG);
    tb3d->addWidget(new QLabel("Z"));
    tb3d->addAction(ui->actionZD);
    connect(ui->actionZD, &QAction::triggered, this, &MainWindow::tourner3DZD);

    //tb3d->addSeparator();
    //tb3d->addAction(ui->actionZoomMoins);
    //connect(ui->actionZoomMoins, &QAction::triggered, this, &MainWindow::zoom3DMoins);
    //tb3d->addWidget(new QLabel("Zoom."));
    //tb3d->addAction(ui->actionZoomPlus);
    //connect(ui->actionZoomPlus, &QAction::triggered, this, &MainWindow::zoom3DPlus);

    ui->verticalLayout3D->setMenuBar(tb3d);

    // menu 2d
    QToolBar *tb2d = new QToolBar(this);

/*    tb2d->addAction(ui->action2DZoomMoins);
    //connect(ui->action2DZoomMoins, &QAction::triggered, this, &MainWindow::zoom2DMoins);
    tb2d->addWidget(new QLabel("Zoom"));
    tb2d->addAction(ui->action2DZoomPlus);
    //connect(ui->action2DZoomPlus, &QAction::triggered, this, &MainWindow::zoom2DPlus);

    tb2d->addSeparator();
    tb2d->addAction(ui->action2DRotMoins);
    //connect(ui->action2DRotMoins, &QAction::triggered, this, &MainWindow::Rot2DMoins);
    tb2d->addWidget(new QLabel("Rot."));
    tb2d->addAction(ui->action2DRotPlus);
    //connect(ui->action2DRotPlus, &QAction::triggered, this, &MainWindow::Rot2DPlus);
*/
    tb2d->addSeparator();
    leEchelle =  new QLineEdit();
    leEchelle->setMaximumWidth(50);
    QDoubleValidator *val = new QDoubleValidator();
    val->setLocale(QLocale::C);
    val->setNotation(QDoubleValidator::StandardNotation);
    val->setRange(0.1f, 100.f);
    leEchelle->setValidator(val);
    tb2d->addWidget(new QLabel("Echelle:"));
    tb2d->addWidget(leEchelle);
    connect(leEchelle, &QLineEdit:: returnPressed, this, &MainWindow::changeEchelle);

    tb2d->addSeparator();
    tb2d->addWidget(new QLabel("Lang.:"));
    cbLanguettes = new QComboBox();
    cbLanguettes->addItems({"Sans", "1/paire", "2/paire"});
    cbLanguettes->setMaximumWidth(100);
    tb2d->addWidget(cbLanguettes);
    connect(cbLanguettes, &QComboBox::currentIndexChanged, this, &MainWindow::changeTypeLang);

    tb2d->addSeparator();
    tb2d->addAction(ui->actionPgMoins);
    tb2d->addAction(ui->actionPgPlus);
    connect(ui->actionPgPlus, &QAction::triggered, this, &MainWindow::ajoutePage);
    connect(ui->actionPgMoins, &QAction::triggered, this, &MainWindow::supprimePage);

    tb2d->addSeparator();
    tb2d->addWidget(new QLabel("Marges:"));
    QComboBox *cbMarges = new QComboBox();
    cbMarges->addItems({"Sans", "Cricut", "Brother", "Silhoutte"});
    cbMarges->setMaximumWidth(150);
    tb2d->addWidget(cbMarges);
    connect(cbMarges, &QComboBox::currentIndexChanged, this, &MainWindow::changeMarge);

    ui->verticalLayout2D->setMenuBar(tb2d);

    scene3d = new DepliageScene(this, false);
    ui->vue3d->setScene(scene3d);

    scene2d = new DepliageScene(this);
    ui->vue2d->setScene(scene2d);
}
