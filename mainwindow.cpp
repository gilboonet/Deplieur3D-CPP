#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QTableWidget>
#include <QGraphicsScene>
#include <QFileDialog>
#include <QToolBar>
#include <QColorDialog>
#include <QLabel>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{    
    ui->setupUi(this);
    setWindowFlag(Qt::Window);

    dep = nullptr;

    // Menu principal
    connect(ui->actionNouveau, &QAction::triggered, this, &MainWindow::nouveau);
    connect(ui->actionQuitter, &QAction::triggered, this, &MainWindow::quitter);

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

    tb3d->addWidget(new QLabel("x"));

    tb3d->addAction(ui->actionXD);
    connect(ui->actionXD, &QAction::triggered, this, &MainWindow::tourner3DXD);

    tb3d->addSeparator();

    tb3d->addAction(ui->actionYG);
    connect(ui->actionYG, &QAction::triggered, this, &MainWindow::tourner3DYG);

    tb3d->addWidget(new QLabel("y"));

    tb3d->addAction(ui->actionYD);
    connect(ui->actionYD, &QAction::triggered, this, &MainWindow::tourner3DYD);

    tb3d->addSeparator();

    tb3d->addAction(ui->actionZG);
    connect(ui->actionZG, &QAction::triggered, this, &MainWindow::tourner3DZG);

    tb3d->addWidget(new QLabel("y"));

    tb3d->addAction(ui->actionZD);
    connect(ui->actionZD, &QAction::triggered, this, &MainWindow::tourner3DZD);

    tb3d->addSeparator();

    tb3d->addAction(ui->actionZoomMoins);
    connect(ui->actionZoomMoins, &QAction::triggered, this, &MainWindow::zoom3DMoins);

    tb3d->addWidget(new QLabel("ech"));

    tb3d->addAction(ui->actionZoomPlus);
    connect(ui->actionZoomPlus, &QAction::triggered, this, &MainWindow::zoom3DPlus);

    ui->verticalLayout3D->setMenuBar(tb3d);

    // menu 2d
    QToolBar *tb2d = new QToolBar(this);

    tb2d->addAction(ui->action2DZoomMoins);
    connect(ui->action2DZoomMoins, &QAction::triggered, this, &MainWindow::zoom2DMoins);

    tb2d->addWidget(new QLabel("ech"));

    tb2d->addAction(ui->action2DZoomPlus);
    connect(ui->action2DZoomPlus, &QAction::triggered, this, &MainWindow::zoom2DPlus);

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
}

void MainWindow::nouveau()
{
    auto fileContentReady = [this](const QString &fileName, const QByteArray &fileContent) {
        if (!fileName.isEmpty()) {
            // Use fileName and fileContent
            dep = new Depliage(this);
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
            dep->creeFaces2d();
            dep->trouveVoisinage();
            ui->vue3d->fitInView(dep->scene3d->itemsBoundingRect(), Qt::KeepAspectRatio);
            dep->basculeSelectionChanged(true);
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
    }
    else if (nbSel < 5) {
        QStringList si;
        for(auto && i : scene->selectedItems())
            si.append(i->data(0).toString());
        statusBar()->showMessage("faces: " + si.join(","));
    }
    else {
        statusBar()->showMessage(QString("%1 faces").arg(nbSel));
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
                //auto v = dep->meshModel->faces[ni].voisins;
                //qDebug() << "sel2D : " << ni << " - " << nj << " (" << v[0].nF << ", " << v[1].nF << ", " << v[2].nF << ")";
                j->setSelected(true);
            }
        }
    }
    dep->basculeSelectionChanged(true);
}

void MainWindow::couleurChoisie (QColor color)
{
    if (color.isValid()) {
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
    }
}

void MainWindow::couleurNouveau()
{
    QColorDialog *dialog = new QColorDialog();
    dialog->setOption(QColorDialog::NoEyeDropperButton);
    connect(dialog, &QColorDialog::colorSelected, this, [this](const QColor& color) {couleurChoisie(color);});
    dialog->open();
}

void MainWindow::couleurSupprime()
{
    int n = ui->tableCouleurs->currentRow();

    if (n == 0) {
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

void MainWindow::couleurAssemble()
{
    int n = ui->tableCouleurs->currentRow();
    if (n == 0)
        return;   
    if (dep->pool[n].elements.size() == 0)
        return;

    QList<int> wPool = dep->pool[n].elements;

    dep->pool[n].pieces.clear();

    // Commencer avec la 1ère face
    std::vector<Attache> att0;
    att0.push_back(Attache(-1, wPool.takeFirst())); // commencer avec la 1ère face
    dep->pool[n].pieces.push_back(att0);

    int nbKO = 0;
    bool ok = false;
    while (wPool.size() > 0) { //  Boucler sur ce qui est posé
        //int np = 1;
        for (auto&& p : dep->pool[n].pieces) {
            for (auto&& piece : p) {
                int wP = piece.vers;
                ok = false;
                for(auto&& v : dep->meshModel->faces[wP].voisins) {
                    if (wPool.contains(v.nF)) {
                        p.push_back(Attache(wP, v.nF));
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

        if (nbKO >= 2) { // créer nouvelle pièce
            std::vector<Attache> att1;
            att1.push_back(Attache(-1, wPool.takeFirst())); // commencer avec la 1ère face
            dep->pool[n].pieces.push_back(att1);
            nbKO = 0;
            ok = false;
        }
    }

    // Rattachement des triangles par piece
    int nP = 1;
    for (auto&& p : dep->pool[n].pieces) {
        qDebug() << "piece " << nP++;
        for (auto&& piece : p) {
            //auto v = dep->meshModel->faces[piece.vers].voisins;
            //qDebug() << piece.de << "->" << piece.vers << " (" << v[0].nF << ", " << v[1].nF << ", " << v[2].nF << ")";
            if (piece.de == -1) { // debut de piece, commencer le polygone

            }
        }
    }

    int nb = 1;
    for (auto&& p : dep->pool[n].pieces) {
        qDebug() << "Piece " <<  nb++;
        for (auto&& piece : p) {
            qDebug() << "de " << piece.de << " vers " << piece.vers;
        }
    }
}

void MainWindow::couleurClic(int r, int c) {
    if (c != 0 || dep == nullptr) {
        return;
    }

    int nbF = dep->meshModel->faces.size();
    bool need2dRefresh = false;

    for(auto &item:dep->scene2d->selectedItems()) {
        int i = item->data(0).toInt();
        if (i < nbF){
            int aC = dep->meshModel->faces[i].col;
            if (aC != r) {
                dep->meshModel->faces[i].col = r;
                dep->t2d[i]->col = r;
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
    dep->fThetaX += dep->fPas;
    dep->dessineModele();
}

void MainWindow::tourner3DXG()
{
    dep->fThetaX -= dep->fPas;
    dep->dessineModele();
}

void MainWindow::tourner3DYD()
{
    dep->fThetaY += dep->fPas;
    dep->dessineModele();
}

void MainWindow::tourner3DYG()
{
    dep->fThetaY -= dep->fPas;
    dep->dessineModele();
}

void MainWindow::tourner3DZD()
{
    dep->fThetaZ += dep->fPas;
    dep->dessineModele();
}

void MainWindow::tourner3DZG()
{
    dep->fThetaZ -= dep->fPas;
    dep->dessineModele();
}

void MainWindow::zoom3DPlus()
{
    ui->vue3d->scale(1.1f, 1.1f);
}

void MainWindow::zoom3DMoins()
{
    ui->vue3d->scale(0.9f, 0.9f);
}

void MainWindow::zoom2DPlus()
{
    ui->vue2d->scale(1.1f, 1.1f);
}

void MainWindow::zoom2DMoins()
{
    ui->vue2d->scale(0.9f, 0.9f);
}
