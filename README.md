#  Deplieur3D-CPP
Déplie un volume en sélectionnant des pièces par coloration des faces grâce à une UI en 3D

C'est un projet QT C++, construit (CMAKE) avec QT Creator (14.0.2)
(kit Desktop MingGW Qt 6.8.0 ou Desktop MSVC Qt 6.8.0, ou  WebAssembly Qt 6.8.0)

# (23-11.24)
## FAIT
- Crer un nouveau projet de dépliage (ouvrir un fichier 3D, format .obj)
- Afficher le modèle 3D, le gabarit 2D multipage par défaut
- Tourner/zoomer le modèle 3D
- Sélectionner les faces du modèle 3D
- Déplacer/zoomer les pages du gabarit
- Sélectionner les faces depuis le gabarit
- Synchroniser les sélections 3D et 2D
- Déplacer les faces dans leur page
- Ajouter/Supprimer des couleurs (1 couleur = 1 ou plusieurs pièces)
- Renommer les couleurs
- Appliquer une couleur à une sélection de faces
- Centrer le modèle 3d dans la vue
- Attacher les faces d'une pièce

## A FAIRE
- Déplacer les pièces d'une page à une autre
- Tourner les faces/pièces
- Couper/attacher des faces/pièces
- Sauver/Charger un projet
- Exporter le gabarit en SVG (peut-être aussi en PDF)
- Voir/modifier les dimensions du projet
- Changer les paramètres de (mise en) page du projet(format, orientation, marges)
- (peut-être) Sélectionner toutes les faces d'une couleur

## Si vous voulez tester la version Wasm (tous navigateurs)
https://gilboonet.github.io/deplieur2/Deplieur.html
(fichiers .obj ici : https://github.com/gilboonet/gilboonet.github.io/tree/master/modeles)

## Raccourcis clavier (AZERTY)

- rotation du modèle 3D
- Axe X axis avec "W" et "C" (Touches avant et après le "X")
- Axe Y axis avec "T" et "U" (Touches avant et après le "Y")
- Axe Z axis avec "A" et "E" (Touches avant et après le "Z")
- Dézoomer avec "Q", zoomer avec "S"

## Problèmes WebAssembly rencontrés
- Appuyer sur "TAB" si un raccourci clavier ne répond pas (généralement après avoir ajouté une couleur)
