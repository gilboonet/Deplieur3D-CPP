// Interface de l'application Deplieur
//---------------------------------------------------------
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "depliage.h"
#include "depliagescene.h"
#include "depliagevue3d.h"
#include "depliagevue2d.h"
#include "filedownloader.h"

#include <QMainWindow>
#include <QStatusBar>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QTableWidget>
#include <QSplitter>
//---------------------------------------------------------

QList<QList<QPointF>> PtsDepuisLignesDeCoupe(Piece *piece);
void bascule (QObject *);
QString strToHex(QString);
QLabel* creeColorLabel (QColor);

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QStringList gCOL;
    Depliage dep;
    DepliageScene *scene3d, *scene2d;
    FileDownloader *m_demoFichier;
    //bool demoMode = false;

private:
    QLineEdit *leEchelle = nullptr;
    QComboBox *cbLanguettes = nullptr;
    QComboBox *cbDemo = nullptr;
    DepliageVue3d *vue3d = nullptr;
    DepliageVue2d *vue2d = nullptr;
    QTableWidget *tableCouleurs = nullptr;
    QSplitter *splitter = nullptr;
    QStatusBar *statusbar = nullptr;
    bool chargementEnCours = false;

    const QString nomApp = "Deplieur 3D v0.2 (28-02-25)";

    QPainterPath construitChemin(QList<QLineF>);
    void triangle3dSetHover(bool);

private slots:
    void resizeEvent (QResizeEvent*) override;
    void keyPressEvent(QKeyEvent *event) override;

    //void demo ();
    //void doDemo ();
    //void lanceDemo (int);

    void projetNouveau ();
    void projetSauve ();
    void projetCharge ();
    void projetExporte ();

    void chargeFichier ();

    void basculeVue (int);
    void basculeCouleurs ();
    void bascule3d ();
    void bascule2d ();

    void couleurChoisie (QColor);
    void couleurNouveau ();

    void ajuste3D ();
    void ajuste2D ();

    void pageAjoute ();
    void pageSupprime ();

    void couleurClic (int, int);
    void changeCouleur (int);
    void changeNBCouleur (int, int = 0);
    void changeFaceCouleur (int, int);
    void peutColorierFace (int, int = -1);

    void pieceAjouteFace(int, int, bool = false);
    bool pieceEnleveFace (int);
    bool pieceEnleveFaces(int, int);
    void pieceCreeLignes (Piece*);
    void pieceMAJ (Piece *);
    void pieceMAJCentre(Piece *);
    void piecesMAJ ();
    void pieceScission(int, int);
    void pieceHoverOn (int);
    void pieceHoverOff (int);

    void ligneHoverOn(int, int);
    void face3dMAJ (Piece *, int);
    void tableNumerote ();

    void tourne3D (qreal, qreal, qreal);
    void tourne2D (qreal, QGraphicsItem *);

    void changeEchelle();
    void changeMarge (int);

    void changeTypeLang (int);
    void basculeLanguette(int, int);

    void facetteHoverOn (int);
    void facetteHoverOff (int);
};
#endif // MAINWINDOW_H
