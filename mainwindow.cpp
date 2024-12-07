#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QTableWidget>
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QFileDialog>
#include <QToolBar>
#include <QColorDialog>
#include <QLabel>
#include <QTransform>
#include "triangleligneitem.h"
#include "trianglelangitem.h"
#include "numitem.h"
#include <QtSvg/QSvgGenerator>
#include <QBuffer>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{    
    ui->setupUi(this);
    setWindowFlag(Qt::Window);

    dep = nullptr;

    // Menu principal
    QToolBar *tbMain = new QToolBar(this);
    tbMain->addAction(ui->actionNouveau);
    tbMain->addAction(ui->actionOuvrir);
    tbMain->addAction(ui->actionSauver);
    tbMain->addAction(ui->actionExporter);
    tbMain->addAction(ui->actionQuitter);
    tbMain->setOrientation(Qt::Vertical);
    tbMain->setMaximumWidth(25);
    ui->verticalLayoutMenu->addWidget(tbMain);

    connect(ui->actionNouveau, &QAction::triggered, this, &MainWindow::nouveau);
    connect(ui->actionQuitter, &QAction::triggered, this, &MainWindow::quitter);
    connect(ui->actionExporter, &QAction::triggered, this, &MainWindow::exporter);

    // Menu Pieces/couleurs
    QToolBar *tbCol = new QToolBar(this);
    tbCol->addAction(ui->actionCouleurPlus);
    connect(ui->actionCouleurPlus, &QAction::triggered, this, &MainWindow::couleurNouveau);

    tbCol->addAction(ui->actionCouleurMoins);
    connect(ui->actionCouleurMoins, &QAction::triggered, this, &MainWindow::couleurSupprime);

    tbCol->addAction(ui->actionCouleurAssembler);
    connect(ui->actionCouleurAssembler, &QAction::triggered, this, &MainWindow::couleurAssemble);

    ui->verticalLayoutCouleurs->setMenuBar(tbCol);

    ui->tableCouleurs->setRowCount(1);
    ui->tableCouleurs->setColumnCount(3);
    ui->tableCouleurs->setColumnWidth(0, 5);
    ui->tableCouleurs->setColumnWidth(1, 35);
    ui->tableCouleurs->setColumnWidth(2, 113);

    // Première ligne : couleur white par défaut
    QPixmap pixmap(15, 15);
    QColor c = Qt::white;
    pixmap.fill(c);
    QLabel *l = new QLabel();
    l->setPixmap(pixmap);
    l->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    l->setTextInteractionFlags(Qt::NoTextInteraction);
    l->setFocusPolicy(Qt::NoFocus);
    ui->tableCouleurs->setCellWidget(0, 0, l);
    QTableWidgetItem *tiN = new QTableWidgetItem("0");
    ui->tableCouleurs->setItem(0, 1, tiN);
    QTableWidgetItem *tiT = new QTableWidgetItem("Defaut");
    ui->tableCouleurs->setItem(0, 2, tiT);

    connect(ui->tableCouleurs, &QTableWidget::cellPressed, this, &MainWindow::couleurClic);
    ui->tableCouleurs->clearSelection();

    // Menu 3d
    QToolBar *tb3d = new QToolBar(this);

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

    tb3d->addSeparator();

    tb3d->addAction(ui->actionZoomMoins);
    connect(ui->actionZoomMoins, &QAction::triggered, this, &MainWindow::zoom3DMoins);

    tb3d->addWidget(new QLabel("Zoom."));

    tb3d->addAction(ui->actionZoomPlus);
    connect(ui->actionZoomPlus, &QAction::triggered, this, &MainWindow::zoom3DPlus);

    ui->verticalLayout3D->setMenuBar(tb3d);

    // menu 2d
    QToolBar *tb2d = new QToolBar(this);

    tb2d->addAction(ui->action2DZoomMoins);
    connect(ui->action2DZoomMoins, &QAction::triggered, this, &MainWindow::zoom2DMoins);

    tb2d->addWidget(new QLabel("Zoom"));

    tb2d->addAction(ui->action2DZoomPlus);
    connect(ui->action2DZoomPlus, &QAction::triggered, this, &MainWindow::zoom2DPlus);

    tb2d->addSeparator();
    tb2d->addAction(ui->action2DRotMoins);
    connect(ui->action2DRotMoins, &QAction::triggered, this, &MainWindow::Rot2DMoins);

    tb2d->addWidget(new QLabel("Rot."));

    tb2d->addAction(ui->action2DRotPlus);
    connect(ui->action2DRotPlus, &QAction::triggered, this, &MainWindow::Rot2DPlus);

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
    connect(leEchelle, &QLineEdit::returnPressed, this, &MainWindow::changeEchelle);

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

    ui->verticalLayout2D->setMenuBar(tb2d);
}

MainWindow::~MainWindow()
{
    if (dep) {
        dep->scene3d->disconnect();
        dep->scene2d->disconnect();
        free(dep);
    }
    delete ui;
}

void MainWindow::quitter()
{
    QApplication::quit();
}

void MainWindow::ajoutePage() {
    if (!dep)
        return;

    dep->ajoutePage();
    ui->vue2d->fitInView(dep->scene2d->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void MainWindow::supprimePage() {
    if (!dep)
        return;

    if (!dep->pages.count() > 1) {
        dep->scene2d->removeItem(dep->pages.last());
        dep->pages.removeLast();
        ui->vue2d->fitInView(dep->scene2d->itemsBoundingRect(), Qt::KeepAspectRatio);
    }
}

void MainWindow::changeTypeLang(int index) {
    if (!dep)
        return;

    TLang iL = static_cast<TLang>(index);

    if (iL == L00 || iL == L11) {
        for (auto&& num : dep->nums) {
            num.tlang = iL;
        }
    } else {
        for (auto&& num : dep->nums) {
            if (num.tlang != L10 && num.tlang != L01)
                num.tlang = L10;
        }
    }
    couleurAssemble();
}

void MainWindow::exporter()
{
    if (!dep)
        return;

    qreal s = 5.0f / 1.76f;

    QSvgGenerator SG;
    SG.setSize(QSize(210*s, 297*s));
    SG.setViewBox(QRect(0, 0, 210*s, 297*s));
    SG.setTitle("EXPORT SVG");
    //SG.setDescription(tr("An SVG drawing created by the SVG Generator "
    //                            "Example provided with Qt."));

    QPainter painter;
    QBuffer buffer;
    SG.setOutputDevice(&buffer);

    QPen penC = QPen(QBrush(Qt::red), 0.2, Qt::SolidLine);
    QPen penM = QPen(QBrush(Qt::darkRed), 0.1, Qt::DashLine);
    QPen pen0 = QPen(QBrush(Qt::black), 0.1, Qt::NoPen);
    QPen penV = QPen(QBrush(Qt::green), 0.1, Qt::DashDotDotLine);
    QPen penN = QPen(QBrush(Qt::blue), 0.2, Qt::SolidLine);

    painter.begin(&SG);
    QFont tf = QFont("Bitstream Vera Sans", 7*s);
    tf.setLetterSpacing(QFont::AbsoluteSpacing, -2);
    painter.setFont(tf);

    QTransform tS;
    tS.scale(s, s);

    for (auto && pool : dep->pool) {
        for (auto && piece : pool.pieces) {
            TriangleItem *prem = dep->t2d[piece.premId];
            for (auto && ligne : prem->childItems()) {
                TriangleLigneItem *tli = qgraphicsitem_cast<TriangleLigneItem*>(ligne);
                if (tli) {
                    QPointF p1 = tS.map(prem->mapToScene(tli->mapToParent(tli->line().p1())));
                    QPointF p2 = tS.map(prem->mapToScene(tli->mapToParent(tli->line().p2())));
                    bool lb = tli->data(1).toBool();
                    if (tli->ligne->nb == 1 && !lb) {
                        // LIGNE DE COUPE
                        painter.setPen(penC);
                    } else {
                        // PLI
                        if (tli->ligne->cop > 0) {
                            // PLI MONTAGNE
                            painter.setPen(penM);
                        } else if (tli->ligne->cop < 0) {
                            // PLI VALLEE
                            painter.setPen(penV);
                        } else {
                            painter.setPen(tli->ligne->nb == 1 ? penC : pen0);
                        }
                    }
                    painter.drawLine(p1, p2);

                    continue;
                }

                TriangleLangItem *tlai = qgraphicsitem_cast<TriangleLangItem*>(ligne);
                if (tlai) {
                    if (tlai->data(1).toBool()) {
                        painter.setPen(penC);
                        painter.drawPath(tS.map(tlai->mapToScene(tlai->path())));
                    }
                    continue;
                }

                NumItem *numi = qgraphicsitem_cast<NumItem*>(ligne);
                if (numi) {
                    painter.setPen(penN);
                    QPointF b, centre;
                    if (numi->bLang) {
                        centre = numi->tli->mapToScene(centroid4(lineToLang(
                                                        numi->tli->line().p1(),
                                                        numi->tli->line().p2()
                                                        ).toFillPolygon()));
                        centre = tS.map(centre);
                        painter.save();
                        painter.translate(centre);
                        painter.rotate(180- numi->tli->line().angle());
                        b = QPointF(- numi->boundingRect().width()/2, 3);
                        painter.drawText(tS.map(b), numi->text());
                        painter.restore();
                    } else {
                        centre =  tS.map(numi->tli->mapToScene(numi->tli->line().center()));
                        painter.save();
                        painter.translate(centre);
                        painter.rotate(180- numi->tli->line().angle());
                        b = QPointF(- numi->boundingRect().width()/2, -1.8f);
                        painter.drawText(tS.map(b), numi->text());
                        painter.restore();
                    }
                }
            }
        }
    }
    painter.end();

    QFileDialog::saveFileContent(buffer.data(), "myExport.svg");
}

void MainWindow::nouveau()
{
    auto fileContentReady = [this](const QString &fileName, const QByteArray &fileContent) {
        if (!fileName.isEmpty()) {
            // Use fileName and fileContent
            dep = new Depliage(this);
            leEchelle->setText(QString::number(dep->echelle,'g',2));
            ui->vue3d->setScene(dep->scene3d);
            ui->vue2d->setScene(dep->scene2d);
            QFileInfo f(fileName);
            setWindowTitle("Deplieur [" + f.fileName() + "]");
            dep->meshModel->LoadFromObjectFile(fileContent);
            QTableWidgetItem *twi = new QTableWidgetItem(QString::number(dep->meshModel->faces.size()));
            ui->tableCouleurs->setItem(0, 1, twi);

            Pool p0;
            QList<int> li;
            for(int i=0; i< dep->meshModel->faces.size(); i++) {
                li.push_back(i);
            }
            p0.couleur = Qt::white;
            p0.elements = li;
            dep->pool.push_back(p0);
            ui->tableCouleurs->setRowCount(1);
            dep->dessineModele();
            connect(dep->scene3d, &DeplieurScene::changeCouleur, this, &MainWindow::changeCouleur);
            dep->creeFaces2d();
            connect(dep->scene2d, &DeplieurScene::changeCouleur, this, &MainWindow::changeCouleur);
            dep->trouveVoisinage();
            ui->vue3d->fitInView(dep->scene3d->itemsBoundingRect(), Qt::KeepAspectRatio);
            dep->basculeSelectionChanged(true);

            vec3d d = dep->meshModel->dim.Vector_Mul(50);
            ui->statusbar->showMessage(QString("Dim : %1 %2 %3").arg(d.x, 0, 'f', 2).arg(d.y, 0, 'f', 2).arg(d.z, 0, 'f', 2));
        }
    };
    QFileDialog::getOpenFileContent("fichier OBJ (*.obj)",  fileContentReady);
}

void MainWindow::SelectionDansScene3D()
{
    if (!dep)
        return;

    AfficheNbSel(dep->scene3d);

    dep->basculeSelectionChanged(false);
    dep->scene2d->clearSelection();
    for(auto&& i : dep->scene3d->selectedItems()) {
        int ni = i->data(0).toInt();
        for(auto && j : dep->t2d) {
            if (ni == j->id) {
                j->setSelected(true);
                break;
            }
        }
    }
    dep->basculeSelectionChanged(true);
}

void MainWindow::AfficheNbSel(QGraphicsScene *scene)
{
    if (!dep)
        return;

    int nbSel = scene->selectedItems().size();
    if (nbSel == 0) {
        statusBar()->showMessage("Aucune selection");
    } else if (nbSel == 1) {
        QList<QGraphicsItem*> si = scene->selectedItems();
        int nf = si[0]->data(0).toInt();
        facette *f = &(dep->meshModel->faces[nf]);

        statusBar()->showMessage( QString("face %1 : v1 (%2,%3) v2(%4,%5) v3(%6,%7)").arg(nf)
            .arg(f->voisins[0].nF).arg(f->voisins[0].pnF)
            .arg(f->voisins[1].nF).arg(f->voisins[1].pnF)
            .arg(f->voisins[2].nF).arg(f->voisins[2].pnF)
        );
    } else if (nbSel < 5) {
        QStringList si;
        for(auto && i : scene->selectedItems())
            si.append(i->data(0).toString());
        statusBar()->showMessage("faces: " + si.join(","));
    } else {
        statusBar()->showMessage(QString("%1 faces").arg(nbSel));
    }
}

void MainWindow::changeCouleur(int couleur)
{
    if (!dep)
        return;

    ui->tableCouleurs->selectRow(couleur);
}

void MainWindow::changeEchelle()
{
    if (!dep)
        return;

    bool ok;
    qreal n = leEchelle->text().toFloat(&ok);

    if (ok) {
        dep->echelle = n;
        for (auto&& i : dep->t2d) {
            if (!(i->parentItem()))
                i->setScale(n);
        }
    }
    dep->scene2d->update();
    vec3d d = dep->meshModel->dim.Vector_Mul(50*n);
    ui->statusbar->showMessage(QString("Dim : %1 %2 %3").arg(d.x, 0, 'f', 2).arg(d.y, 0, 'f', 2).arg(d.z, 0, 'f', 2));
}

void MainWindow::SelectionDansScene2D()
{
    if (!dep)
        return;

    AfficheNbSel(dep->scene2d);

    dep->basculeSelectionChanged(false);
    dep->scene3d->clearSelection();
    for(auto&& i : dep->scene2d->selectedItems()) {
        int ni = i->data(0).toInt();
        for(auto && j : dep->scene3d->items()) {
            int nj = j->data(0).toInt();
            if (ni == nj) {
                j->setSelected(true);
            }
        }
    }
    dep->basculeSelectionChanged(true);
}

void MainWindow::couleurChoisie (QColor color)
{
    if (!dep)
        return;

    if (!color.isValid())
        return;

    bool ok = true;
    for (auto && pc :dep->pool) {
        if (pc.couleur == color) {
            ui->statusbar->showMessage("Couleur déjà utilisée !");
            ok = false;
            break;
        }
    }
    if (ok) {
        Pool p1;
        p1.couleur = color;
        p1.elements = QList<int>();
        dep->pool.push_back(p1);
        int nb = ui->tableCouleurs->rowCount();
        ui->tableCouleurs->setRowCount(nb+1);
        QPixmap pixmap(15, 15);
        pixmap.fill(color);
        QLabel *l = new QLabel();
        l->setPixmap(pixmap);
        l->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
        l->setTextInteractionFlags(Qt::NoTextInteraction);
        l->setFocusPolicy(Qt::NoFocus);
        ui->tableCouleurs->setCellWidget(nb, 0, l);
        QTableWidgetItem *tiN = new QTableWidgetItem("0");
        ui->tableCouleurs->setItem(nb, 1, tiN);
        QString s = QString("Piece #%1").arg(nb);
        QTableWidgetItem *tiT = new QTableWidgetItem(s);
        ui->tableCouleurs->setItem(nb, 2, tiT);
        ui->tableCouleurs->selectRow(nb);
        couleurClic(nb, 0);
    }
}

void MainWindow::couleurNouveau()
{
    if(!dep)
        return;

    QColorDialog *dialog = new QColorDialog();
    dialog->setOption(QColorDialog::NoEyeDropperButton);
    connect(dialog, &QColorDialog::colorSelected, this, [this](const QColor& color) {couleurChoisie(color);});
    dialog->open();
}

void MainWindow::couleurSupprime()
{
    if(!dep)
        return;

    int n = ui->tableCouleurs->currentRow();

    if (n < 1) {
        return;
    }

    // mettre les facettes de cette couleur au défaut

    // supprimer la couleur
    ui->tableCouleurs->removeRow(n);
    dep->pool.remove(n);

    for (auto&& i : dep->meshModel->faces) {
        if (i.col == n) {
            i.col = 0;
        } else if (i.col > n) {
            i.col--;
        }
    }
    for (auto&& i : dep->t2d) {
        if (i->col == n) {
            i->col = 0;
        } else if (i->col > n) {
            i->col--;
        }
    }

    dep->dessineModele();
    dep->scene2d->update();
}

void MainWindow::commencePose(QList<int>*pool, int n)
{
    if (!dep)
        return;

    Piece att;
    att.attaches.push_back(Attache(-1, pool->takeFirst()));
    int nF = att.attaches[0].vers;
    att.premId = nF;
    dep->pool[n].pieces.push_back(att);

    TriangleItem *TI = dep->t2d[nF];
    TI->estLie = false;
    TI->estPrem = true;
    TI->setFlag(QGraphicsItem::ItemIsMovable);
    TI->setZValue(3);
    QPolygonF P = TI->polygon();
    QList<Ligne> L = dep->pool[n].pieces[0].lignes;
    for (int i = 0; i < 3; i++) {
        QPointF P1 = P[i];
        QPointF P2 = P[next(i)];
        Voisin V = dep->meshModel->faces[nF].voisins[i];
        int nl = -1;
        int il = 0;
        for (auto&& R : L) {
            if (   (eq(R.p1, P1) && eq(R.p2, P2))
                || (eq(R.p1, P2) && eq(R.p2, P1))) {
                nl = il;
                break;
            }
            il++;
        }
        if (nl > -1) {
            dep->pool[n].pieces[0].lignes[nl].nb++;
        } else {
            Ligne li (P1, P2, nF, V.nF, V.cop);
            dep->pool[n].pieces[0].lignes.append(li);
        }
    }
}

void MainWindow::couleurAssemble()
{
    if(!dep)
        return;

    dep->scene2d->clearSelection();
    if (ui->tableCouleurs->rowCount() < 2)
        return;

  dep->nums.clear();
  for (int n = 1; n < ui->tableCouleurs->rowCount(); n++) {
    if (dep->pool[n].elements.size() == 0)
          continue;

    QList<int> wPool = dep->pool[n].elements;

    for (auto&& p : dep->pool[n].pieces) {
        for (auto && piece : p.attaches) {
            int wP = piece.vers;
            TriangleItem *tSource = dep->t2d[wP];
            for (auto && ti : tSource->childItems()) {
                if (ti->data(0).toInt() < 0)
                    delete(ti);
            }
        }
    }

    dep->pool[n].pieces.clear();

    // Commencer avec la 1ère face
    commencePose(&wPool, n);

    int nbKO = 0;
    bool ok = false;
    int idParentCourant = -1;
    while (wPool.size() > 0) { //  Boucler sur ce qui est posé
        for (auto&& p : dep->pool[n].pieces) {
            for (auto && piece : p.attaches) {
                int wP = piece.vers;
                TriangleItem *tSource = dep->t2d[wP];
                tSource->setFiltersChildEvents(true);
                if (piece.de == -1){
                    idParentCourant = piece.vers;
                    QPointF tpos = tSource->scenePos();
                    tSource->setParentItem(0);
                    tSource->setPos(tpos);
                }
                ok = false;
                for(auto&& v : dep->meshModel->faces[wP].voisins) {
                    if (wPool.contains(v.nF)) {
                        p.attaches.push_back(Attache(wP, v.nF));
                        TriangleItem *tCible = dep->t2d[v.nF];
                        tCible->setVisible(true);
                        tCible->estLie = true;
                        tCible->estPrem = false;
                        tCible->setParentItem(dep->t2d[idParentCourant]);
                        tCible->setZValue(2);
                        tCible->setFlag(QGraphicsItem::ItemIsMovable);
                        QPolygonF pCible = tCible->polygon();
                        QPolygonF pSource = tSource->polygon();
                        QPointF ptOrig = pSource[v.id];
                        QPointF delta = ptOrig - pCible[v.idx];
                        QTransform trT;
                        trT.translate(delta.x(), delta.y());
                        pCible = trT.map(pCible);

                        QTransform trR;
                        qreal angle = calc_angle(
                            ptOrig,
                            pSource[next(v.id)],
                            pCible[prev(v.idx)]);
                        trR.translate(ptOrig.x(), ptOrig.y());
                        trR.rotate(-angle);
                        trR.translate(-ptOrig.x(), -ptOrig.y());

                        tCible->setPolygon(trR.map(pCible));
                        tCible->update();

                        QPolygonF P = tCible->polygon();
                        QList<Ligne> L = p.lignes;
                        for (int i = 0; i < 3; i++) {
                            QPointF P1 = P[i];
                            QPointF P2 = P[next(i)];
                            Voisin V = dep->meshModel->faces[v.nF].voisins[i];
                            int nl = -1;
                            int il = 0;
                            for (auto&& R : L) {
                                if (   (eq(R.p1, P1) && eq(R.p2, P2))
                                    || (eq(R.p1, P2) && eq(R.p2, P1))) {
                                    nl = il;
                                    break;
                                } else
                                    il++;
                            }
                            if (nl > -1) {
                                p.lignes[nl].nb++;
                            } else {
                                Ligne li (P1, P2, V.nF, V.pnF, V.cop);
                                p.lignes.append(li);
                            }
                        }

                        wPool.remove(wPool.indexOf(v.nF));
                        ok = true;
                        break;
                    }
                }
                if (ok) {
                    break;
                }
            }
            if (ok) {
                break;
            }
        }

        if (!ok)
            nbKO++;

        if (nbKO > 1) { // créer nouvelle pièce
            commencePose(&wPool, n);
            nbKO = 0;
            ok = false;
        }
    }

    bool lb;
    for (auto&& P : dep->pool[n].pieces) {
        TriangleItem *ti0 = dep->t2d[P.attaches[0].vers];
        for (auto&& L : P.lignes) {
            TriangleLigneItem *li = new TriangleLigneItem(&L);
            li->setData(1, QVariant::fromValue(0));
            li->setParentItem(ti0);
            if (L.nb == 1) {
                int numC;
                Nums num1 = Nums(L.id1, L.id2);
                Nums num2 = Nums(L.id2, L.id1);
                switch (cbLanguettes->currentIndex()) {
                case 0:
                case 1:
                    lb = false;
                    break;
                case 2:
                    lb = true;
                }

                //QPointF cLai(0,0);
                TriangleLangItem *pLai = nullptr;
                if (dep->nums.contains(num1)) {
                    numC = dep->nums[dep->nums.indexOf(num1)].num;
                    TriangleLangItem *lai = new TriangleLangItem(&L);
                    lai->setParentItem(ti0);
                    //lai->setParentItem(li);
                    lai->setData(0, QVariant::fromValue(-1));
                    lai->setData(1, QVariant(lb));
                    li->setData(1, QVariant(lb));
                } else
                if (dep->nums.contains(num2)) {
                    numC = dep->nums[dep->nums.indexOf(num2)].num;
                    TriangleLangItem *lai = new TriangleLangItem(&L);
                    lai->setParentItem(ti0);
                    //lai->setParentItem(li);
                    lai->setData(0, QVariant::fromValue(-1));
                    lai->setData(1, QVariant(lb));
                    li->setData(1, QVariant(lb));
                } else {
                    numC = dep->nums.size();
                    num1.num = numC;
                    switch (cbLanguettes->currentIndex()) {
                    case 1 :
                        num1.tlang = L10;
                        lb = true;
                        break;
                    case 2 :
                        num1.tlang = L11;
                        lb = true;
                        break;
                    case 0 :
                        num1.tlang = L00;
                        lb = false;
                    }
                    dep->nums.append(num1);
                    TriangleLangItem *lai = new TriangleLangItem(&L);
                    lai->setParentItem(ti0);
                    //lai->setParentItem(li);
                    lai->setData(0, QVariant::fromValue(-1));
                    lai->setData(1, QVariant(lb));
                    li->setData(1, QVariant(lb));
                    pLai = lai;
                }
                NumItem *numi = new NumItem(li, pLai, numC, dep->dYt, lb);
                numi->setParentItem(ti0);

            }
        }
    }
  }
}

void MainWindow::couleurClic(int r, int c) {
    if (c != 0 || !dep) {
        return;
    }

    int nbF = dep->meshModel->faces.size();
    bool need2dRefresh = false;

    QList<int> LO;
    for(auto &item:dep->scene3d->selectedItems()) {
        LO.append(item->data(0).toInt());
    }
    std::sort(LO.begin(), LO.end());

    for(int i : LO) {
        //int i = item->data(0).toInt();
        //TriangleItem *item = dep->t2d[i];
        if ( i >= 0 && i < nbF) {
            int aC = dep->meshModel->faces[i].col;
            if (aC != r) {
                dep->meshModel->faces[i].col = r;
                dep->t2d[i]->col = r;
                dep->t2d[i]->poolColor = dep->pool[r].couleur;
                dep->t2d[i]->estLie = false;
                dep->t2d[i]->estPrem = false;
                //dep->t2d[i]->setParentItem(0);
                dep->t2d[i]->setPos(dep->t2d[i]->mapToScene(dep->t2d[i]->pos()));
                need2dRefresh = true;
                dep->pool[r].elements.push_back(i);
                auto aSupprimer = dep->pool[aC].elements.indexOf(i);
                if (aSupprimer > -1) {
                    dep->pool[aC].elements.remove(aSupprimer);
                }
                QTableWidgetItem *twia = new QTableWidgetItem(QString::number(dep->pool[aC].elements.size()));
                ui->tableCouleurs->setItem(aC, 1, twia);
            }
        }
    }
    QTableWidgetItem *twi = new QTableWidgetItem(QString::number(dep->pool[r].elements.size()));
    ui->tableCouleurs->setItem(r, 1, twi);

    dep->dessineModele();
    if (need2dRefresh)
        dep->scene2d->update();
}

void MainWindow::tourner3DXD()
{
    if(!dep)
        return;

    dep->fThetaX += dep->fPas;
    dep->dessineModele();
}

void MainWindow::tourner3DXG()
{
    if(!dep)
        return;

    dep->fThetaX -= dep->fPas;
    dep->dessineModele();
}

void MainWindow::tourner3DYD()
{
    if(!dep)
        return;

    dep->fThetaY += dep->fPas;
    dep->dessineModele();
}

void MainWindow::tourner3DYG()
{
    if(!dep)
        return;

    dep->fThetaY -= dep->fPas;
    dep->dessineModele();
}

void MainWindow::tourner3DZD()
{
    if(!dep)
        return;

    dep->fThetaZ += dep->fPas;
    dep->dessineModele();
}

void MainWindow::tourner3DZG()
{
    if(!dep)
        return;

    dep->fThetaZ -= dep->fPas;
    dep->dessineModele();
}

void MainWindow::zoom3DPlus()
{
    if(!dep)
        return;

    ui->vue3d->scale(1.1f, 1.1f);
}

void MainWindow::zoom3DMoins()
{
    if(!dep)
        return;

    ui->vue3d->scale(0.9f, 0.9f);
}

void MainWindow::zoom2DPlus()
{
    if(!dep)
        return;

    ui->vue2d->scale(1.1f, 1.1f);
}

void MainWindow::zoom2DMoins()
{
    if(!dep)
        return;

    ui->vue2d->scale(0.9f, 0.9f);
}

void MainWindow::Rot2D(int a) {
    if (!dep)
        return;

    auto sI = dep->scene2d->selectedItems();
    if (sI.size() < 1)
        return;

    int ni = sI[0]->data(0).toInt();
    if (ni < 0)
        return;

    TriangleItem *ti = dep->t2d[ni];

    QPolygonF p = ti->polygon();
    QPointF c = centroid(p);
    QTransform tr;
    tr.translate(c.x(), c.y()).rotate(a).translate(-c.x(), -c.y());

    if (!ti->estPrem && !ti->estLie) {
        dep->t2d[ti->id]->setPolygon(tr.map(p));
    } else {
        if (ti->estLie) {
            ti = dep->t2d[ti->parentItem()->data(0).toInt()];
        }
        // tourne la première face
        p = ti->polygon();
        ti->setPolygon(tr.map(p));

        // tourne les enfants
        dep->t2d[ti->id]->setPolygon(tr.map(p));

        for (auto&& i : ti->childItems()) {
            TriangleItem *te = qgraphicsitem_cast<TriangleItem*>(i);
            if (te) {
                if (te->col == ti->col){
                    p = te->polygon();
                    te->setPolygon(tr.map(p));
                }
                continue;
            }

            TriangleLigneItem * li = qgraphicsitem_cast<TriangleLigneItem*>(i);
            if (li) {
                QLineF lil = li->line();
                li->setLine(tr.map(lil));
                continue;
            }

            TriangleLangItem * tli = qgraphicsitem_cast<TriangleLangItem*>(i);
            if (tli) {
                QPainterPath path;
                path = tli->path();
                tli->setPath(tr.map(path));
            }
        }

        for (auto&& i : ti->childItems()) {
            NumItem *ci = qgraphicsitem_cast<NumItem*>(i);
            if (ci) {
                ci->tourne();
            }
        }
    }
}

void MainWindow::Rot2DPlus()
{
    Rot2D(1);
}

void MainWindow::Rot2DMoins()
{
    Rot2D(-1);
}
