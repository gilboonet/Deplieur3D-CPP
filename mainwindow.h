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
    QComboBox *cbDemo = nullptr;

    QPainterPath construitChemin(QList<QLineF>);

private slots:
    void resizeEvent (QResizeEvent*);

    void nouveau ();
    void demo ();
    void chargeFichier ();
    void lanceDemo (int);
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
    void tourneModele (qreal, qreal, qreal);
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
