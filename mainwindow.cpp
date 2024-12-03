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
    //leEchelle =  new QLineEdit("1.0");
    leEchelle =  new QLineEdit();
    leEchelle->setMaximumWidth(50);
    QDoubleValidator *val = new QDoubleValidator();
    val->setLocale(QLocale::C);
    val->setNotation(QDoubleValidator::StandardNotation);
    val->setRange(0.1f, 100.f);
    leEchelle->setValidator(val);
    tb2d->addWidget(new QLabel("Echelle:"));
    tb2d->addWidget(leEchelle);
    //connect(leEchelle, QLineEdit::textEdited, this, &MainWindow::majEchelle);
    connect(leEchelle, &QLineEdit::returnPressed, this, &MainWindow::changeEchelle);

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

void MainWindow::exporter()
{
    if (!dep)
        return;

    QSvgGenerator SG;
    SG.setSize(QSize(210, 297));
    SG.setViewBox(QRect(0, 0, 210, 297));
    SG.setTitle("EXPORT SVG");
    //SG.setDescription(tr("An SVG drawing created by the SVG Generator "
    //                            "Example provided with Qt."));

    QPainter painter;
    QBuffer buffer;
    SG.setOutputDevice(&buffer);

    QPen pen = QPen();

    painter.begin(&SG);
    painter.setFont(dep->tf);
    for (auto && pool : dep->pool) {
        for (auto && piece : pool.pieces) {
            TriangleItem *prem = dep->t2d[piece.premId];
            for (auto && ligne : prem->childItems()) {
                TriangleLigneItem *tli = qgraphicsitem_cast<TriangleLigneItem*>(ligne);
                if (tli) {
                    QPointF p1 = tli->mapToScene(tli->line().p1());
                    QPointF p2 = tli->mapToScene(tli->line().p2());
                    if (tli->ligne->nb == 1) {
                        // LIGNE DE COUPE
                        pen.setColor(Qt::red);
                        pen.setStyle(Qt::SolidLine);
                    } else {
                        // PLI
                        if (tli->ligne->cop > 0) {
                            // PLI MONTAGNE
                            pen.setColor(Qt::darkRed);
                            pen.setStyle(Qt::DashLine);
                        } else if (tli->ligne->cop < 0) {
                            // PLI VALLEE
                            pen.setColor(Qt::green);
                            pen.setStyle(Qt::DashDotLine);
                        } else {
                            pen.setColor(Qt::black);
                            pen.setStyle(Qt::NoPen);
                        }
                    }
                    painter.setPen(pen);
                    painter.drawLine(p1, p2);

                    for (auto&& litxt : tli->childItems()) {
                        QGraphicsSimpleTextItem *txti = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(litxt);
                        if (txti) {
                            pen.setColor(Qt::blue);
                            pen.setStyle(Qt::SolidLine);
                            painter.setPen(pen);
                            QPointF c = ((p1 + p2) / 2);
                            painter.save();
                            painter.translate(c);
                            painter.rotate(180-tli->line().angle());
                            QPointF b(- txti->boundingRect().width()/2, -1.8f);
                            painter.drawText(b, txti->text());
                            painter.restore();
                        }
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
}

void MainWindow::SelectionDansScene2D()
{
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
            //tSource->setVisible(true);
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
                //tSource->setFiltersChildEvents(true);
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

    for (auto&& P : dep->pool[n].pieces) {
        TriangleItem *ti0 = dep->t2d[P.attaches[0].vers];
        for (auto&& L : P.lignes) {
            TriangleLigneItem *li = new TriangleLigneItem(&L);
            li->setParentItem(ti0);
            if (L.nb == 1) {
                int numC;
                Nums num1 = Nums(L.id1, L.id2);
                Nums num2 = Nums(L.id2, L.id1);
                if (dep->nums.contains(num1)) {
                    numC = dep->nums[dep->nums.indexOf(num1)].num;
                } else
                if (dep->nums.contains(num2)) {
                    numC = dep->nums[dep->nums.indexOf(num2)].num;
                } else {
                    numC = dep->nums.size();
                    num1.num = numC;
                    dep->nums.append(num1);
                }

                QGraphicsSimpleTextItem *ti = new QGraphicsSimpleTextItem(QString::number(numC));
                ti->setFont(dep->tf);
                ti->setParentItem(li);
                QPointF b = QPointF(ti->boundingRect().width()/2, ti->boundingRect().height()+ dep->dYt);
                ti->setTransformOriginPoint(b);
                ti->setRotation(180-li->line().angle());
                QPointF c = li->line().center() - b;
                ti->setPos(c);
                ti->setZValue(3);
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

QPointF centroid(QPolygonF p) {
return QPointF(
    (p[0].x() + p[1].x() + p[2].x()) / 3,
    (p[0].y() + p[1].y() + p[2].y()) / 3
    );
}

void MainWindow::Rot2D(int a) {
    if (!dep)
        return;
    auto sI = dep->scene2d->selectedItems();
    if (sI.size() < 1)
        return;

    TriangleItem *ti = dep->t2d[sI[0]->data(0).toInt()];

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
            int d = i->data(0).toInt();
            if (d < 0) {
                TriangleLigneItem * li = qgraphicsitem_cast<TriangleLigneItem*>(i);
                QLineF lil = li->line();
                li->setLine(tr.map(lil));
                auto ci = li->childItems();
                if (ci.size() > 0) {
                    QPointF b = QPointF(ci[0]->boundingRect().width()/2, ci[0]->boundingRect().height() + dep->dYt);
                    ci[0]->setTransformOriginPoint(b);
                    ci[0]->setRotation(180-li->line().angle());
                    QPointF c = li->line().center() - b;
                    ci[0]->setPos(c);

                }
            } else { // TriangleItem
                TriangleItem *te = dep->t2d[d];
                if (te->col == ti->col){
                    p = te->polygon();
                    te->setPolygon(tr.map(p));
                }
            }
        }
    }
}

void MainWindow::Rot2DPlus()
{
    Rot2D(5);
}

void MainWindow::Rot2DMoins()
{
    Rot2D(-5);
}
