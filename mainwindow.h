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
// QT_BEGIN_NAMESPACE
// namespace Ui {
// class MainWindow;
// }
// QT_END_NAMESPACE

QList<QList<QPointF>> PtsDepuisLignesDeCoupe(Piece *piece);
void bascule (QObject *);
QLabel* creeColorLabel (QColor);

const QStringList gCOL = { "white",
    "Brown", "Cyan", "Crimson", "Chartreuse", "BurlyWood", "Peru", "CadetBlue",
    "Chocolate", "Fuchsia", "Gold", "Coral", "Aquamarine", "Tomato",
    "BlueViolet", "CornflowerBlue", "Maroon", "MediumAquaMarine", "MediumBlue",
    "MediumOrchid", "MediumPurple", "MediumSeaGreen", "MediumSlateBlue",
    "MediumSpringGreen", "Lavender", "MediumTurquoise", "MediumVioletRed",
    "MidnightBlue", "MintCream", "MistyRose", "DarkCyan", "DarkGoldenRod",
    "DarkGray", "DarkKhaki", "DarkMagenta", "DarkOliveGreen", "DarkOrange",
    "DarkOrchid", "DarkSalmon", "DarkSeaGreen", "DarkTurquoise", "DarkViolet",
    "DeepPink", "DeepSkyBlue", "DimGray", "DodgerBlue", "Turquoise", "Violet",
    "FireBrick", "ForestGreen", "GoldenRod", "IndianRed",
    "Indigo", "GreenYellow", "HoneyDew", "HotPink", "Khaki", "YellowGreen",
    "LavenderBlush", "LawnGreen", "LemonChiffon", "LightBlue", "LightCoral",
    "LightCyan", "LightGoldenRodYellow", "LightGray", "Olive",
    "LightGreen", "LightPink", "LightSalmon", "LightSeaGreen", "LightSkyBlue",
    "LightSlateGray", "LightSteelBlue", "LightYellow", "Lime", "LimeGreen",
    "Linen", "Moccasin", "NavajoWhite", "Navy", "OldLace", "Olive", "Yellow",
    "OliveDrab", "Orange", "OrangeRed", "Orchid", "PaleGoldenRod", "Wheat",
    "PaleTurquoise", "PaleVioletRed", "PapayaWhip", "PeachPuff", "WhiteSmoke",
    "Pink", "Plum", "PowderBlue", "Purple", "RosyBrown", "RoyalBlue",
    "SaddleBrown", "Salmon", "SandyBrown", "SeaGreen", "SeaShell", "Sienna",
    "Silver", "SkyBlue", "SlateBlue", "SlateGray", "Snow", "SpringGreen",
    "SteelBlue", "Tan", "Teal", "Thistle"
};

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
    //Ui::MainWindow *ui;
    QLineEdit *leEchelle = nullptr;
    QComboBox *cbLanguettes = nullptr;
    QComboBox *cbDemo = nullptr;
    DepliageVue3d *vue3d = nullptr;
    DepliageVue2d *vue2d = nullptr;
    QTableWidget *tableCouleurs = nullptr;
    QSplitter *splitter = nullptr;
    QStatusBar *statusbar = nullptr;

    const QString nomApp = "Deplieur 3D v0.1 (09-01-25)";

    QPainterPath construitChemin(QList<QLineF>);

private slots:
    void resizeEvent (QResizeEvent*) override;
    void keyPressEvent(QKeyEvent *event) override;

    void nouveau ();
    void demo ();
    void doDemo ();
    void chargeFichier ();
    void lanceDemo (int);
    void exporte ();
    void sauveProjet ();

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
