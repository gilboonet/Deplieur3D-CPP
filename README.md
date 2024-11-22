#  Deplieur3D-CPP
Volume unfolder with 3D UI to select faces

It's a Qt C++ project, built with CMAKE on QT Creator (14.0.2)
using Desktop MinGW kit, MSVC Qt 6.8.0 kit or WebAssembly Qt 6.8.0 kit.

(22-11.24)
##DONE
- create a new project (open a 3d model, .obj file)
- rotate/zoom the 3d model
- select faces of the 3d model
- zoom/move the pages (2D)
- select faces from the pages
- move faces within page
- add/remove colors (one color = 1 piece)
- naming color
- apply color to selected faces

##TODO
- Join pieces faces
- move pieces within page
- rotate faces/pieces
- Cut/stick faces/pieces
- save/load project
- export to svg (and maybe pdf)
- view/modify project dimensions
- center 3d model into view
- change project page settings (format, orientation, margins)
- (maybe) select all faces by color

##Current Wasm version
https://gilboonet.github.io/deplieur2/Deplieur.html

##Sample .obj files
https://github.com/gilboonet/gilboonet.github.io/tree/master/modeles

##Current keyboard shortcuts :

- 3D model rotation (AZERTY keyboard)
- rotate X axis with "W" and "C" (keys before and after X key)
- rotate Y axis with "T" and "U" (                      Y    )
- rotate Z axis with "A" and "E" (                      Z    )
- zoom in with "Q" key and zoom out with "S" key

##WebAssembly troubleshooting :
press "TAB" if a keyboard shortcut doesn't respond (usually after a new color was added)
