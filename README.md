#  Deplieur3D-CPP
Déplie un volume en sélectionnant des pièces par coloration des faces grâce à une UI en 3D

C'est un projet QT C++, construit (CMAKE) avec QT Creator (15.0.1)
(kit Desktop Qt 6.8.2 ou WebAssembly Qt 6.8.2)

# (14-02-25)
## FAIT
- Crer un nouveau projet de dépliage (ouvrir un fichier 3D, format .obj)
- Afficher le modèle 3D, le gabarit 2D multipage par défaut
- Tourner ou zoomer sur le modèle 3D (à la souris ou au clavier)
- édition par clic (gauche) sur une face (ajoute à la face courante ou la retire)
- (retirer une face n'est possible que si la face n'a qu'un voisin dans cette pièce)
- (l'ajout d'une face n'est possible que si elle ne chevauche pas une autre face dans la pièce, hors languettes)
- clic droit sur une face pour sélectionner sa couleur
- Zoomer sur le gabarit
- visualiser la face courante du gabarit pointée par la souris sur l'autre vue
- Déplacer/tourner les pièces du gabarit
- clic sur une arête intérieure d'une pièce pour la scinder en deux pièces
- Ajouter une couleur (= 1 pièce)
- Renommer une couleur
- Centrer le modèle 3d dans la vue
- Numéroter les arêtes
- Exporter le gabarit en SVG
- Voir les dimensions du projet (entrer 1 dans paramètre "Echelle" et valider)
- Changer les dimensions du projet
- Voir/cacher les marges pour "Imprimer puis découper"
- Gestion des languettes (paramètre "type lang.")
- clic sur un numéro d'arête pour basculer sa languette
- Sauver un projet (format de fichier .obj.dep compatible avec .obj)
- Edition de face dans la vue 2D (clic sur une arête pour enlever, ajouter ou déplacer une face)
- Charger un projet (format de fichier .obj.dep)
- Ligne témoin entre arêtes pour visualiser leur lien

## A FAIRE
- Changer les paramètres de (mise en) page du projet(format, orientation, marges)

## Si vous voulez tester la version Wasm (tous navigateurs)
https://gilboonet.github.io/deplieur2/Deplieur.html
(fichiers .obj ici : https://github.com/gilboonet/gilboonet.github.io/tree/master/modeles)
