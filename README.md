#  Deplieur3D-CPP
Déplie un volume en sélectionnant des pièces par coloration des faces grâce à une UI en 3D

C'est un projet QT C++, construit (CMAKE) avec QT Creator (14.0.2)
(kit Desktop MingGW Qt 6.8.0 ou Desktop MSVC Qt 6.8.0, ou  WebAssembly Qt 6.8.0)

# (19-12-24)
## FAIT
- Crer un nouveau projet de dépliage (ouvrir un fichier 3D, format .obj)
- Afficher le modèle 3D, le gabarit 2D multipage par défaut
- Tourner ou zoomer sur le modèle 3D (à la souris ou au clavier)
- édition par clic (gauche) sur une face (met/retire la couleur courante)
- clic droit sur une face pour sélectionner sa couleur
- Zoomer sur le gabarit
- visualiser la face courante du gabarit depuis celle pointée par la souris sur la vue 3D
- Déplacer/tourner les pièces du gabarit
- Ajouter/Supprimer des couleurs (1 couleur = 1 pièce)
- Renommer les couleurs
- Centrer le modèle 3d dans la vue
- Numéroter les arêtes
- Exporter le gabarit en SVG
- Voir/modifier les dimensions du projet (paramètre "Echelle")
- Voir/cacher les marges pour "Imprimer puis découper"
- Gestion des languettes (paramètre "type lang.")
- clic sur un numéro d'arête pour basculer sa languette

## A FAIRE
- Sauver/Charger un projet
- Changer les paramètres de (mise en) page du projet(format, orientation, marges)

## Si vous voulez tester la version Wasm (tous navigateurs)
https://gilboonet.github.io/deplieur2/Deplieur.html
(fichiers .obj ici : https://github.com/gilboonet/gilboonet.github.io/tree/master/modeles)
