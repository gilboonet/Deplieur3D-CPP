#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mesh.h"
#include "depliage.h"

#include <QMainWindow>
#include <QGraphicsScene>

class Depliage;


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Depliage *dep;

private:
    Ui::MainWindow *ui;
    void commencePose(QList<int>*, int);
    void Rot2D(int);

private slots:
    void nouveau();
    void quitter();
    void exporter();

    void couleurNouveau();
    void couleurSupprime();
    void couleurAssemble();

    void couleurChoisie (QColor);
    void couleurClic(int, int);


    void tourner3DXG();
    void tourner3DXD();
    void tourner3DYG();
    void tourner3DYD();
    void tourner3DZG();
    void tourner3DZD();
    void zoom3DPlus();
    void zoom3DMoins();

    void zoom2DPlus();
    void zoom2DMoins();
    void Rot2DPlus();
    void Rot2DMoins();

    void AfficheNbSel(QGraphicsScene *);
    void changeCouleur(int);

public slots:
    void SelectionDansScene3D();
    void SelectionDansScene2D();
};

#endif // MAINWINDOW_H
