// Interface de l'application Deplieur
//---------------------------------------------------------
#include "mainwindow.h"
#include "triangleitem3d.h"
#include "triangleitem2d.h"
#include "piecepolygonitem.h"
#include "pieceligneitem.h"
#include "piecelangitem.h"
#include "piecenumitem.h"
#include "svg.hpp"

#include <QApplication>
#include <QVBoxLayout>
#include <QToolBar>
#include <QToolButton>
#include <QMenu>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QLocale>
#include <QDebug>
//---------------------------------------------------------
QString strToHex(QString ch) {
    QString ret;

    for (int i = 0; i < ch.length(); i++) {
        QString ch1 = ch.mid(i, 1);
        QByteArray n = ch1.toLatin1();
        QByteArray h = n.toHex();
        ret.append(QString("%1").arg(h));
    }

    return ret;
}
//---------------------------------------------------------
QString hexToStr(QString ch) {
    QString ret;

    QByteArray b = QByteArray::fromHex(ch.toLatin1());
    ret = b.data();

    return ret;
}
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
    //delete ui;
}

void MainWindow::resizeEvent (QResizeEvent* event) {
    ajuste3D();
    QMainWindow::resizeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {

    switch(event->key()) {
    case Qt::Key_A :
        tourneModele(dep.fPas, 0, 0);
        return;
    case Qt::Key_E :
        tourneModele(-dep.fPas, 0, 0);
        return;
    case Qt::Key_W :
        tourneModele(0, dep.fPas, 0);
        return;
    case Qt::Key_C :
        tourneModele(0, -dep.fPas, 0);
        return;
    case Qt::Key_T :
        tourneModele(0, 0, dep.fPas);
        return;
    case Qt::Key_U :
        tourneModele(0, 0, -dep.fPas);
        return;
    }

    QMainWindow::keyPressEvent (event);
}

void MainWindow::changeEchelle()
{
    QString ch = leEchelle->text();
    bool ok;

    qreal n = ch.toFloat(&ok);
    if (!ok) {
        ch = ch.replace(",", ".");
        n = ch.toFloat(&ok);
    }

    if (ok && (n != 0)) {
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

    if (n != 0) {
        dep.dim = dep.dim.Vector_Mul(n);
    }

    vec3d d = dep.dim;
    statusbar->showMessage(QString("Dim : %1 %2 %3").arg(d.x, 0, 'f', 0).arg(d.y, 0, 'f', 0).arg(d.z, 0, 'f', 0));
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
    tableCouleurs->selectRow(couleur);
    couleurClic(couleur, 0);
}

void MainWindow::changeNBCouleur (int ligne, int delta) {
    Piece *piece = &(dep.pieces[ligne]);
    piece->nb = piece->nb + delta;
    QTableWidgetItem *twi = new QTableWidgetItem(QString::number(piece->nb));
    tableCouleurs->setItem(ligne, 2, twi);
}

void MainWindow::changeFaceCouleur (int faceId, int couleurId) {
    dep.faces[faceId].col = couleurId;
}

bool MainWindow::pieceEnleveFaces(int faceId1, int faceId2) {
    if (pieceEnleveFace(faceId1))
        return true;
    else
        return pieceEnleveFace(faceId2);
}

bool MainWindow::pieceEnleveFace (int faceId) {
    Facette *facette = &(dep.faces[faceId]);
    int pieceId = facette->col;
    Piece *piece = &(dep.pieces[pieceId]);
    TriangleItem2d *tCible = facette->triangleItem;
    QColor blanc = QColor(Qt::white);
    int nb = 0;

    qDebug() << "Peut enlever" << faceId;
    for (auto && e : piece->elements2)
        qDebug() << QString("%1 %2").arg(e.de).arg(e.vers);


    bool ok = false;
    if (piece->elements2.constFirst().vers == faceId)
        ok = true;
    else if (piece->elements2.constLast().vers == faceId)
        ok = true;
    else {
        for (auto && e : piece->elements2) {
            if ((e.de == faceId) || ((e.de > -1) && (e.vers == faceId))) {
                nb++;
            }
        }
        if (nb < 2) {
            ok = true;
        }
    }

    if (!ok) {
        statusbar->showMessage("Impossible (>1 voisin) !");
        return false;
    }

    if (piece->elements.removeOne(faceId)) {
        //Attache att(faceId);
        //piece->elements2.removeOne(att);
        for (auto &&el : piece->elements2) {
            if (el.vers == faceId) {
                piece->elements2.removeOne(el);
                break;
            }
        }

        tCible->setParentItem(0);
        if (tCible->hoverOn)
            hoverOff(tCible->id);
        QPointF delta = piece->bord->pos();
        tCible->moveBy(-delta.x(), -delta.y());
        tCible->setPos(0,0);

        changeNBCouleur(pieceId, -1);
        changeNBCouleur(0, +1);

        for (auto && i : piece->elements2) {
            if (i.de == faceId)
                i.de = -1;
        }
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
        statusbar->showMessage("Face enlevée !");
        scene3d->update();
    }
    return true;
}

void MainWindow::pieceCreeLignes (Piece *piece) {
    piece->lignes.clear();

    QPointF delta = dep.faces[piece->elements.first()].triangleItem->pos();
    QTransform transform;
    transform.translate(delta.x(), delta.y());
    Nums n1, nF;
    // int nC;
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
                    // nC = dep.chercheNum(ligne.id1, ligne.id2);
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
    QPolygonF tmpP;
    int coulCourante = tableCouleurs->currentRow();
    Voisin vT;

    if (piece->nb == 0) { // piece vide -> créer groupe + ajouter 1ère face
        if (!piece->bord) {
            piece->bord = new PiecePolygonItem(scene2d, piece->couleur);
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
        // 1°) Essayer à partir de la dernière facette parcourue
        if ((scene3d->dernFace > -1) && (scene3d->dernFace < dep.faces.size())) {
            Facette face = dep.faces[scene3d->dernFace];
            if (face.col == coulCourante) {
                for (Voisin v : face.voisins) {
                    if (v.nF == faceId) {
                        voisinTrouve = true;
                        vT = v;
                        break;
                    }
                }
            }
        }

        if (!voisinTrouve) {
            // 2°) Chercher en parcourant la pièce courante
            for (auto it = piece->elements.constBegin(); it != piece->elements.constEnd(); ++it) {
                Facette face = dep.faces[*it];
                if (face.col == coulCourante) {
                    for (Voisin v : face.voisins) {
                        if (v.nF == faceId) {
                            voisinTrouve = true;
                            vT = v;
                            break;
                        }
                    }
                }
                if (voisinTrouve)
                    break;
            }
        }

        if (!voisinTrouve) {
            // 2°) Chercher en parcourant la face
            for (Voisin v : facette->voisins) {
                Facette vFace = dep.faces[v.nF];
                qDebug() << v.id << v.idx << v.nF << v.pnF;
                if (vFace.col == coulCourante) {
                    voisinTrouve = true;
                    vT = v;
                    break;
                }
            }
        }

        if (voisinTrouve) {
            statusbar->showMessage("Ok");
            tSource = dep.faces[vT.pnF].triangleItem;

            QPolygonF pCible = tCible->polygon();
            QPolygonF pSource = tSource->polygon();
            QPointF ptOrig = pSource[vT.id];
            QPointF delta = ptOrig - pCible[vT.idx];
            QTransform trT;
            trT.translate(delta.x(), delta.y());
            pCible = trT.map(pCible);

            QTransform trR;
            qreal angle = calc_angle(
                ptOrig,
                pSource[next(vT.id)],
                pCible[prev(vT.idx)]);
            trR.translate(ptOrig.x(), ptOrig.y());
            trR.rotate(-angle);
            trR.translate(-ptOrig.x(), -ptOrig.y());

            ok = true;
            tmpP = trR.map(pCible);
            for (auto it = piece->elements.constBegin(); it < piece->elements.constEnd(); ++it) {
                const int& ni = *it;
                if ((ni >= 0) && (ni< dep.faces.size())) {
                    QPolygonF t = dep.faces[ni].triangleItem->polygon();
                    QPolygonF ptest = tmpP.intersected(t);
                    if (ptest.size() > 2) {
                        if (ptest.size() == 4) {
                            QString t0, t1, t2, t3;
                            t0 = QString("%1 %2").arg(ptest[0].x(), 0, 'f', 2).arg(ptest[0].y(), 0, 'f', 2);
                            t1 = QString("%1 %2").arg(ptest[1].x(), 0, 'f', 2).arg(ptest[1].y(), 0, 'f', 2);
                            t2 = QString("%1 %2").arg(ptest[2].x(), 0, 'f', 2).arg(ptest[2].y(), 0, 'f', 2);
                            t3 = QString("%1 %2").arg(ptest[3].x(), 0, 'f', 2).arg(ptest[3].y(), 0, 'f', 2);
                            ok = (t0 == t1) && (t0 == t2) && (t0 == t3);
                        } else
                            ok = false;

                        if (!ok)
                            qDebug() << "chevauchement" << ptest.size();

                        break;
                    }
                }
            }

            if (!ok) {
                statusbar->showMessage("Impossible (chevauchement) !");
            }
            else {
                tCible->setPolygon(tmpP);
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
                statusbar->showMessage("Face ajoutée !");
            }
        }
        else {
            statusbar->showMessage("voisin non trouvé");
        }
    }
    if (ok) {
        qDebug() << "Piece" << piece->id << "ajoute" << faceId;
        for (auto && e : piece->elements2)
            qDebug() << QString("%1 %2").arg(e.de).arg(e.vers);

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
        PieceLigneItem *gli = new PieceLigneItem(scene2d, piece->bord, &l, num);
        gli->setParentItem(piece->bord);
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

void MainWindow::peutColorierFace (int faceId, int faceSource) {
    // CLIC SUR UNE FACE 3D : COLORIER OU NON ?
    Facette *facette = &(dep.faces[faceId]);
    int coul;
    if (faceSource == -1)
        coul = tableCouleurs->currentRow();
    else {
        coul = dep.faces[faceSource].col;
        scene3d->dernFace = faceSource;
        tableCouleurs->selectRow(coul);
    }

    // SI COUL = 0
    // -> SI COUL FACE = 0 -> ne rien faire
    // ----> SINON ENLEVER FACE DE SA PIECE ACTUELLE
    if (coul == 0) {
        if (facette->col == 0) {
            //qDebug() << "RIEN A FAIRE";
        } else {
            //qDebug() << "ENLEVER DE PIECE (coul = " << facette->col << ")";
            pieceEnleveFace(facette->id);
        }
    } else {
        if (facette->col == 0) {
            qDebug() << "AJOUTER " << facette->id << " A PIECE (" << coul << ")";
            pieceAjouteFace(coul, facette->id);
        } else {
            //qDebug() << "ENLEVER DE PIECE (coul = " << facette->col << ")";
            if (pieceEnleveFace(facette->id)) {
                //qDebug() << "AJOUTER A PIECE (coul = " << coul << ")";
                pieceAjouteFace(coul, facette->id);
            }
        }
    }
}

void MainWindow::exporte () {
    if (!dep.ModeleOK)
        return;

    SVG::SVG root;
    QByteArray svgRoot;

    qreal s = 5.0f / 1.76f;
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
    QFileDialog::saveFileContent(svgRoot, "MonExport.svg");
}

void MainWindow::sauveProjet()
{
    if (!dep.ModeleOK)
        return;

    QByteArray qbSauve;
    QStringList sauve;
    QString ligne;

    // 0°) "header"
    sauve.append("# cree par Deplieur 3D");
    sauve.append("o OBJ");
    // 1°) sauve donnees du modele 3D

    // vecteurs (triangles 3d)

    // rech. max
    int vmax = 0;
    for (auto && f : dep.faces) {
        vmax = std::max({vmax, f.pi[0], f.pi[1], f.pi[2]});
    }

    vec3d vnv;
    for (int nv = 0; nv <= vmax; nv++) {
        bool ok = false;
        for (auto && f : dep.faces) {
            if (f.pi[0] == nv) {
                vnv = f.p[0];
                ok = true;
                break;
            }
            if (f.pi[1] == nv) {
                vnv = f.p[1];
                ok = true;
                break;
            }
            if (f.pi[2] == nv) {
                vnv = f.p[2];
                ok = true;
                break;
            }
        }

        if (ok) {
            // PTS 3D (VECTEURS)
            vnv = vnv.Vector_Mul(50);
            ligne = QString("v %1 %2 %3")
                        .arg(vnv.x, 0, 'f', 4)
                        .arg(vnv.y, 0, 'f', 4)
                        .arg(vnv.z, 0, 'f', 4);
            sauve.append(ligne);
        }
    }

    // faces
    for (auto && i : dep.faces) {
        // TRIANGLES (FACES)
        ligne = QString("f %1 %2 %3")
            .arg(1+i.pi[0])
            .arg(1+i.pi[1])
            .arg(1+i.pi[2]);
        sauve.append(ligne);
    }

    sauve.append("#FIN OBJ");

    // 2°) sauve donnees du depliage

    // pieces
    for (auto && p : dep.pieces) {
        if (p.id == 0)
            continue;

        if (p.elements.size() == 0)
            continue;

        // PIECE
        QString libelle = tableCouleurs->item(p.id, 3)->text();
        if (libelle.isEmpty())
            libelle = QString("Piece #%1").arg(p.id);

        ligne = QString("dp %1 %2 %3 %4 %5 %6")
                    .arg(p.id)
                    .arg(p.couleur.name(), p.cDesign.name())
                    .arg(p.bord->x(), 0, 'f', 2)
                    .arg(p.bord->y(), 0, 'f', 2)
                    .arg(strToHex(libelle));
        sauve.append(ligne);

        // ELEMENTS (de PIECE)
        for (auto && pt : p.elements2) {
            ligne = QString("de %1 %2")
            .arg(pt.de)
            .arg(pt.vers);
            sauve.append(ligne);
        }

        // PREMIER ELEMENT (de PIECE)
        int nEl1 = p.elements2.first().vers;
        QPolygonF pEl = dep.faces[nEl1].triangleItem->polygon();
        ligne = QString("d1 %1 %2 %3 %4 %5 %6")
                    .arg(pEl[0].x(), 0, 'f', 2)
                    .arg(pEl[0].y(), 0, 'f', 2)
                    .arg(pEl[1].x(), 0, 'f', 2)
                    .arg(pEl[1].y(), 0, 'f', 2)
                    .arg(pEl[2].x(), 0, 'f', 2)
                    .arg(pEl[2].y(), 0, 'f', 2);
        sauve.append(ligne);
    }

    // numerotation + languettes
    ligne = QString("dl %1").arg(dep.typeLang);
    sauve.append(ligne);

    for (auto && n : dep.nums) {
        ligne = QString("dn %1 %2 %3 %4")
            .arg(n.id1)
            .arg(n.id2)
            .arg(n.num)
            .arg(static_cast<int>(n.tlang));
        sauve.append(ligne);
    }

    ligne = QString("dd %1 %2")
        .arg(dep.dimPage.x())
        .arg(dep.dimPage.y());
    sauve.append(ligne);


    qbSauve = sauve.join('\n').toUtf8();
    QFileDialog::saveFileContent(qbSauve, "monProjet.obj.dep");
}

void MainWindow::chargeProjet()
{
    // Nouveau projet : choisit un fichier .objet.dep et le charge
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
            setWindowTitle(QString("%1 [%2]").arg(nomApp, f.fileName()));
            dep.chargeFichierOBJ(fileContent, true);
            chargeFichier();
            dep.ModeleCharge = true;
        }
    };
    QFileDialog::getOpenFileContent("Projet Deplieur (*.obj.dep)",  fileContentReady);
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

void MainWindow::tourneModele (qreal dZ, qreal dX, qreal dY) {
    dep.fThetaZ += dZ;
    dep.fThetaX -= dX;
    dep.fThetaY += dY;
    dep.dessineModele(scene3d);
}

void MainWindow::tourne2D (qreal a) {
    if (scene2d->selectedItems().isEmpty())
        return;

    PiecePolygonItem *bord = static_cast<PiecePolygonItem*>(scene2d->selectedItems().constFirst());
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
    if (!vue3d)
        return;

    if (vue3d->scene())
        //ui->vue3d->fitInView(ui->vue3d->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
        vue3d->fitInView(vue3d->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void MainWindow::ajuste2D () {
    if (!scene2d->pageTemoin) {
        scene2d->pageTemoin = new QGraphicsRectItem( 0, 0, 220* scene2d->nbPages, 297);
        scene2d->pageTemoin->setPen(QPen(Qt::NoPen));
        scene2d->pageTemoin->setData(0, QVariant(1));
        scene2d->addItem(scene2d->pageTemoin);
    } else
        scene2d->pageTemoin->setRect(0, 0, 220* scene2d->nbPages, 297);
    vue2d->fitInView(vue2d->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void MainWindow::couleurClic (int ligne, int col) {
    // clic sur une ligne du tableau des couleurs
    if (!dep.ModeleOK)
        return;

    scene3d->itemColorId = ligne;
    scene3d->itemColor = dep.pieces[ligne].couleur;
}

void MainWindow::bascule2d () {
    // AFFICHE/CACHE LA VUE 2D
    bascule(splitter->children()[0]);
    ajuste3D();
    ajuste2D();
}

void MainWindow::bascule3d () {
    // AFFICHE/CACHE LA VUE 3D
    bascule(splitter->children()[1]);
    ajuste3D();
    ajuste2D();
}

void MainWindow::basculeCouleurs () {
    // AFFICHE/CACHE LES COULEURS
    bascule(splitter->children()[2]);
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
        int nb = tableCouleurs->rowCount();
        tableCouleurs->setRowCount(nb+1);
        p1.id = nb;
        dep.pieces.append(p1);
        tableCouleurs->setCellWidget(nb, 0, creeColorLabel(color));
        tableCouleurs->setCellWidget(nb, 1, creeColorLabel(color));
        tableCouleurs->setRowHeight(nb, 14);
        tableCouleurs->setItem(nb, 2, new QTableWidgetItem("0"));
        QString s = QString("Piece #%1").arg(nb);
        tableCouleurs->setItem(nb, 3, new QTableWidgetItem(s));
        statusbar->showMessage("Couleur ajoutée");
        tableCouleurs->selectRow(nb);
        couleurClic(nb, 0);
    } else {
        statusbar->showMessage("Couleur déjà utilisée !");
    }
}

void MainWindow::couleurNouveau () {
    // Le bouton "+" (couleurs) a été appuyé ...
    if(!dep.ModeleOK)
        return;

    couleurChoisie(QColor(gCOL[tableCouleurs->rowCount()]));
    // QColorDialog *dialog = new QColorDialog();
    // dialog->setOption(QColorDialog::NoEyeDropperButton);
    // connect(dialog, &QColorDialog::colorSelected, this, [this](const QColor& color) {couleurChoisie(color);});
    // dialog->open();
}

void MainWindow::demo() {
    if (dep.ModeleCharge) {
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setAttribute(Qt::WA_DeleteOnClose);
        msgBox->setIcon(QMessageBox::Question);
        msgBox->setText("Charger ce modèle ?");
        msgBox->setInformativeText("Attention, cela écrasera votre travail");
        msgBox->setStandardButtons(QMessageBox::Open | QMessageBox::Cancel);
        msgBox->setDefaultButton(QMessageBox::Cancel);
        connect(msgBox, &QMessageBox::finished, this, [this](){
            dep.nums.clear();
            dep.pieces.clear();
            dep.faces.clear();
            dep = Depliage();
            dep.ModeleOK = true;
            dep.echelle = 1;
            dep.fPas = 0.1;
            scene2d->nbPages = 1;
            leEchelle->setText(QString::number(dep.echelle, 'g', 2));
            setWindowTitle(QString("%1 [Modele Demo]").arg(nomApp));
            demoMode = true;
            dep.chargeFichierOBJ(m_demoFichier->downloadedData());
            chargeFichier();
            dep.ModeleCharge = true;
        });
        msgBox->show();
    }
    else
        doDemo();
}

void MainWindow::doDemo() {
    dep.nums.clear();
    dep.pieces.clear();
    dep.faces.clear();
    dep = Depliage();
    dep.ModeleOK = true;
    dep.echelle = 1;
    dep.fPas = 0.1;
    scene2d->nbPages = 1;
    leEchelle->setText(QString::number(dep.echelle, 'g', 2));
    setWindowTitle(QString("%1 [Modele Demo]").arg(nomApp));
    demoMode = true;
    dep.chargeFichierOBJ(m_demoFichier->downloadedData());
    chargeFichier();
    dep.ModeleCharge = true;
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
            setWindowTitle(QString("%1 [%2]").arg(nomApp, f.fileName()));
            dep.chargeFichierOBJ(fileContent);
            chargeFichier();
            dep.ModeleCharge = true;
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
    tableCouleurs->setRowCount(1);
    dep.dessineModele(scene3d);
    connect(scene3d, &DepliageScene::changeCouleur, this, &MainWindow::changeCouleur);
    connect(scene3d, &DepliageScene::changeNBCouleur, this, &MainWindow::changeNBCouleur);
    connect(scene3d, &DepliageScene::changeFaceCouleur, this, &MainWindow::changeFaceCouleur);
    connect(vue3d, &DepliageVue3d::tourneModele, this, &MainWindow::tourneModele);
    connect(vue2d, &DepliageVue2d::tourne2D, this, &MainWindow::tourne2D);
    connect(scene3d, &DepliageScene::peutColorierFace, this, &MainWindow::peutColorierFace);
    connect(scene3d, &DepliageScene::pieceEnleveFace, this, &MainWindow::pieceEnleveFace);
    connect(scene2d, &DepliageScene::basculeLanguette, this, &MainWindow::basculeLanguette);
    connect(scene3d, &DepliageScene::hoverOn, this, &MainWindow::hoverOn);
    connect(scene3d, &DepliageScene::hoverOff, this, &MainWindow::hoverOff);

    connect(scene2d, &DepliageScene::peutColorierFace, this, &MainWindow::peutColorierFace);
    connect(scene2d, &DepliageScene::pieceEnleveFace, this, &MainWindow::pieceEnleveFace);
    connect(scene2d, &DepliageScene::pieceEnleveFaces, this, &MainWindow::pieceEnleveFaces);

    scene2d->nbPages = 1;
    dep.creeFaces2d(scene2d);
    dep.trouveVoisinage();

    //connect(dep->scene2d, &DeplieurScene::changeCouleur, this, &MainWindow::changeCouleur);
    //dep.basculeSelectionChanged(true);

    dep.dim = dep.dim.Vector_Mul(50);
    vec3d d = dep.dim;
    statusbar->showMessage(QString("Dim : %1 %2 %3").arg(d.x, 0, 'f', 0).arg(d.y, 0, 'f', 0).arg(d.z, 0, 'f', 0));

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
MainWindow::MainWindow (QWidget *parent) : QMainWindow(parent) {
    // DEFINIT L'INTERFACE

    QString tmpCol = "white, Brown, Cyan, Crimson, Chartreuse, Green, BurlyWood, Yellow, Peru, CadetBlue, Chocolate, Fuchsia, Gold, Coral, Aquamarine, Tomato, BlueViolet, CornflowerBlue, Maroon, MediumAquaMarine, MediumBlue, MediumOrchid, MediumPurple, MediumSeaGreen, MediumSlateBlue, MediumSpringGreen, Lavender, MediumTurquoise, MediumVioletRed, MidnightBlue, MintCream, MistyRose, DarkCyan, DarkGoldenRod, DarkGray, DarkKhaki, DarkMagenta, DarkOliveGreen, DarkOrange, DarkOrchid, DarkSalmon, DarkSeaGreen, DarkTurquoise, DarkViolet, DeepPink, DeepSkyBlue, DimGray, DodgerBlue, Turquoise, Violet, FireBrick, ForestGreen, GoldenRod, IndianRed, Indigo, GreenYellow, HoneyDew, HotPink, Khaki, YellowGreen, LavenderBlush, LawnGreen, LemonChiffon, LightBlue, LightCoral, LightCyan, LightGoldenRodYellow, LightGray, Olive, LightGreen, LightPink, LightSalmon, LightSeaGreen, LightSkyBlue, LightSlateGray, LightSteelBlue, LightYellow, Lime, LimeGreen, Linen, Moccasin, NavajoWhite, Navy, OldLace, Olive, Yellow, OliveDrab, Orange, OrangeRed, Orchid, PaleGoldenRod, Wheat, PaleTurquoise, PaleVioletRed, PapayaWhip, PeachPuff, WhiteSmoke, Pink, Plum, PowderBlue, Purple, RosyBrown, RoyalBlue, SaddleBrown, Salmon, SandyBrown, SeaGreen, SeaShell, Sienna, Silver, SkyBlue, SlateBlue, SlateGray, Snow, SpringGreen, SteelBlue, Tan, Teal, Thistle";
    gCOL.append(tmpCol.split(','));
    tmpCol.clear();

    statusbar = statusBar();
    statusbar->showMessage(nomApp);
    setWindowTitle(nomApp);
    setWindowFlag(Qt::Window);

    dep.ModeleOK = false;

    QFont font = QFont();
    font.setPointSize(8);
    setFont(font);

    QWidget *wMenu = new QWidget(this);
    QWidget *wCouleurs = new QWidget(this);
    QWidget *w3D = new QWidget(this);
    QWidget *w2D = new QWidget(this);
    QVBoxLayout *VBLMenu = new QVBoxLayout(wMenu);
    QVBoxLayout *VBLCouleurs = new QVBoxLayout(wCouleurs);
    QVBoxLayout *VBL3D = new QVBoxLayout(w3D);
    QVBoxLayout *VBL2D = new QVBoxLayout(w2D);

    // Menu principal
    QToolBar *tbMain = new QToolBar(this);
    tbMain->setVisible(true);
    QAction * action;

    action = new QAction(QIcon(":/resources/note_add.png"), "Nouveau projet", this);
    action->setEnabled(true);
    tbMain->addAction(action);
    connect(action, &QAction::triggered, this, &MainWindow::nouveau);

    action = new QAction(QIcon(":/resources/file_open.png"), "Ouvrir projet", this);
    action->setEnabled(true);
    tbMain->addAction(action);
    connect(action, &QAction::triggered, this, &MainWindow::chargeProjet);

    action = new QAction(QIcon(":/resources/file_save.png"), "Sauver projet", this);
    action->setEnabled(true);
    tbMain->addAction(action);
    connect(action, &QAction::triggered, this, &MainWindow::sauveProjet);

    action = new QAction(QIcon(":/resources/file_export.png"), "Exporter en SVG", this);
    action->setEnabled(true);
    tbMain->addAction(action);
    connect(action, &QAction::triggered, this, &MainWindow::exporte);

    action = new QAction(QIcon(":/resources/close.png"), "Quitter", this);
    action->setEnabled(true);
    tbMain->addAction(action);
    connect(action, &QAction::triggered, this, &QApplication::quit);

    tbMain->addSeparator();
    action = new QAction("C", this);
    action->setCheckable(true);
    action->setChecked(true);
    tbMain->addAction(action);
    connect(action, &QAction::triggered, this, &MainWindow::basculeCouleurs);

    action = new QAction("3d", this);
    action->setCheckable(true);
    action->setChecked(true);
    tbMain->addAction(action);
    connect(action, &QAction::triggered, this, &MainWindow::bascule3d);

    action = new QAction("2d", this);
    action->setCheckable(true);
    action->setChecked(true);
    tbMain->addAction(action);
    connect(action, &QAction::triggered, this, &MainWindow::bascule2d);

    tbMain->setOrientation(Qt::Vertical);
    tbMain->setMaximumWidth(25);
    VBLMenu->addWidget(tbMain);

    // Menu Pieces/couleurs
    QToolBar *tbCol = new QToolBar(this);

    action = new QAction("+", this);
    action->setToolTip("Ajouter Couleur/Pièce");
    tbCol->addAction(action);
    connect(action, &QAction::triggered, this, &MainWindow::couleurNouveau);

    //tbCol->addAction(ui->actionCouleurMoins);
    //connect(ui->actionCouleurMoins, &QAction::triggered, this, &MainWindow::couleurSupprime);

    tbCol->setVisible(true);
    VBLCouleurs->setMenuBar(tbCol);

    tableCouleurs = new QTableWidget(1, 4, this);
    tableCouleurs->setColumnWidth(0, 5);
    tableCouleurs->setColumnWidth(1, 5);
    tableCouleurs->setColumnWidth(2, 35);
    tableCouleurs->setColumnWidth(3, 108);
    // Première ligne : couleur white par défaut
    tableCouleurs->setCellWidget(0, 0, creeColorLabel(Qt::white));
    tableCouleurs->setCellWidget(0, 1, creeColorLabel(Qt::white));
    tableCouleurs->setItem(0, 2, new QTableWidgetItem("0"));
    tableCouleurs->setItem(0, 3, new QTableWidgetItem("Defaut"));
    tableCouleurs->setRowHeight(0, 14);
    connect(tableCouleurs, &QTableWidget::cellPressed, this, &MainWindow::couleurClic);
    tableCouleurs->clearSelection();
    tableCouleurs->horizontalHeader()->hide();
    tableCouleurs->verticalHeader()->hide();
    tableCouleurs->setMaximumWidth(190);
    tableCouleurs->setFrameStyle(QFrame::Panel);
    tableCouleurs->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableCouleurs->setVisible(true);
    VBLCouleurs->addWidget(tableCouleurs);

#ifdef Q_OS_WASM
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
    tb3d->setVisible(true);
    VBL3D->setMenuBar(tb3d);
#endif

    // menu 2d
    QToolBar *tb2d = new QToolBar(this);

    leEchelle =  new QLineEdit();
    leEchelle->setMaximumWidth(50);
    // QDoubleValidator *val = new QDoubleValidator();
    // val->setLocale(QLocale::C);
    // val->setNotation(QDoubleValidator::StandardNotation);
    // val->setRange(0.1f, 100.f);
    // leEchelle->setValidator(val);
    tb2d->addWidget(new QLabel("Echelle:"));
    tb2d->addWidget(leEchelle);
    connect(leEchelle, &QLineEdit::returnPressed, this, &MainWindow::changeEchelle);

    tb2d->addSeparator();

    tb2d->addWidget(new QLabel("Lang.:"));
    cbLanguettes = new QComboBox();
    cbLanguettes->addItems({"Sans", "1/paire", "2/paire"});
    cbLanguettes->setMaximumWidth(100);
    tb2d->addWidget(cbLanguettes);
    connect(cbLanguettes, &QComboBox::currentIndexChanged, this, &MainWindow::changeTypeLang);

    tb2d->addSeparator();

    action = new QAction("+", this);
    action->setToolTip("Ajouter Page");
    tb2d->addAction(action);
    connect(action, &QAction::triggered, this, &MainWindow::ajoutePage);

    action = new QAction("-", this);
    action->setToolTip("Supprimer Page");
    tb2d->addAction(action);
    connect(action, &QAction::triggered, this, &MainWindow::supprimePage);

    tb2d->addSeparator();
    tb2d->addWidget(new QLabel("Marges:"));
    QComboBox *cbMarges = new QComboBox();
    cbMarges->addItems({"Sans", "Cricut", "Brother", "Silhoutte"});
    cbMarges->setMaximumWidth(150);
    tb2d->addWidget(cbMarges);
    connect(cbMarges, &QComboBox::currentIndexChanged, this, &MainWindow::changeMarge);

    tb2d->setVisible(true);
    VBL2D->setMenuBar(tb2d);

    vue3d = new DepliageVue3d(this);
    scene3d = new DepliageScene(this, false);
    vue3d->setScene(scene3d);
    vue3d->setVisible(true);
    VBL3D->addWidget(vue3d);

    vue2d = new DepliageVue2d(this);
    scene2d = new DepliageScene(this);
    vue2d->setScene(scene2d);
    vue2d->setVisible(true);
    VBL2D->addWidget(vue2d);

    splitter = new QSplitter(Qt::Horizontal, this);

    VBLMenu->setContentsMargins(1,1,1,1);
    VBLMenu->setSizeConstraint(QLayout::SetMaximumSize);
    splitter->addWidget(wMenu);

    VBLCouleurs->setContentsMargins(1,1,1,1);
    VBLCouleurs->setSizeConstraint(QLayout::SetMaximumSize);
    tableCouleurs->setMaximumWidth(190);
    splitter->addWidget(wCouleurs);

    w3D->setContentsMargins(1,1,1,1);
    vue3d->setContentsMargins(1,1,1,1);
    splitter->addWidget(w3D);

    w2D->setContentsMargins(1,1,1,1);
    vue2d->setContentsMargins(1,1,1,1);
    splitter->addWidget(w2D);

    splitter->setSizes({50,190,500,500});
    splitter->setMinimumSize(1024,600);
    splitter->setHandleWidth(3);
    splitter->setStyleSheet("QSplitter::handle{background: #00aaff;}");

    setCentralWidget(splitter);
}
