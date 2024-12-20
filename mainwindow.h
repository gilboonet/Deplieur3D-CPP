// Interface de l'application Deplieur
//---------------------------------------------------------
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "depliage.h"
#include "depliagescene.h"
#include "filedownloader.h"

#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
//---------------------------------------------------------
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

QList<QList<QPointF>> PtsDepuisLignesDeCoupe(Piece *piece);
void bascule (QObject *);
QLabel* creeColorLabel (QColor);

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Depliage dep;
    DepliageScene *scene3d, *scene2d;
    FileDownloader *m_demoFichier;
    bool demoMode = false;

private:
    Ui::MainWindow *ui;
    QLineEdit *leEchelle = nullptr;
    QComboBox *cbLanguettes = nullptr;

    QPainterPath construitChemin(QList<QLineF>);

private slots:
    void resizeEvent (QResizeEvent*);

    void nouveau ();
    void demo ();
    void chargeFichier ();
    void lanceDemo (QString);
    void lanceDemo1 ();
    void lanceDemo2 ();
    void lanceDemo3 ();
    void lanceDemo4 ();
    void lanceDemo5 ();
    void lanceDemo6 ();
    void lanceDemo7 ();
    void lanceDemo8 ();
    void lanceDemo9 ();
    void lanceDemo10 ();
    void lanceDemo11 ();
    void lanceDemo12 ();
    void lanceDemo13 ();
    void lanceDemo14 ();
    void lanceDemo15 ();
    void lanceDemo16 ();
    //void lanceDemo17 ();
    //void lanceDemo18 ();
    //void lanceDemo19 ();

    void exporte ();

    void basculeCouleurs ();
    void bascule3d ();
    void bascule2d ();

    void couleurChoisie (QColor);
    void couleurNouveau ();

    void ajuste3D ();
    void ajuste2D ();

    void ajoutePage ();
    void supprimePage ();

    void couleurClic (int, int);
    void changeCouleur (int);
    void changeNBCouleur (int, int = 0);
    void changeFaceCouleur (int, int);
    void peutColorierFace (int);
    void pieceAjouteFace (int, int);
    void pieceEnleveFace (int, int);
    void pieceCreeLignes (Piece*);
    void pieceMAJ (Piece *);
    void piecesMAJ ();
    void face3dMAJ (Piece *, int);

    void tourner3DXD ();
    void tourner3DXG ();
    void tourner3DYD ();
    void tourner3DYG ();
    void tourner3DZD ();
    void tourner3DZG ();
    void tourneModele (qreal, qreal);
    void tourne2D (qreal);

    void clicPli ();

    void changeEchelle();
    void changeMarge (int);

    void changeTypeLang (int);
    void basculeLanguette(int, int);

    void hoverOn(int);
    void hoverOff(int);
};
#endif // MAINWINDOW_H
