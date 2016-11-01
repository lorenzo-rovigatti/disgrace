# disgrace

Disgrace is a cross-platform, qt5-based plotting tool. It is an (incomplete) xmgrace clone. I have been using xmgrace since I started doing 
research, and many of its features cannot be found in any other plotting tool. My idea is to rebuild an xmgrace-like tool with a modern 
graphic toolkit (qt5) and with a more modern software architecture (taking into account modularity, plugins, _etc_). I am aware that a qt 
version of xmgrace is being developed (see [here](https://sourceforge.net/projects/qtgrace/) for more details). While this huge effort is 
surely commendable, I believe that a complete rewrite is necessary. 

## Installation
Disgrace requires cmake, qt5 and boost::property_tree. On ubuntu it boils down to installing
* cmake
* qtbase5-dev
* libboost-dev

Once all the dependencies are met, the code can be compiled as follows:
* ```mkdir build```
* ```cd build```
* ```cmake ..```
* ```make```
If the compilation is successful, the disgrace executable will be placed in the build/bin folder. 

## Features
* Undo/redo
* Disgrace should be able to read in sets from files passed as command-line arguments
* Disgrace files should also contain the data sets
* Disgrace should be able to parse xmgrace .agr files. Maybe also write them?
* Heat maps should be supported
* It should be possible to operate on data sets (interpolations, regressions and most of the operations available in the _data_ xmgrace menu)
* Make it (to some degree) scriptable. This should be possibly by a judicious use of [Qt Script](http://doc.qt.io/qt-5/qtscript-index.html)
* It should be possible to open multiple agr files as panels of the same plot   
* Autosave

## Tentative roadmap
* Add basic set appearance management
* Add save/load facilities
* Add plot-wide appearance management (plot size, legend, etc.)
* Add basic data set manipulation facilities
* Add support for multiple plots

## Acknowledgements
* The agr parser is heavily based on [xmgrace_parser.py](https://github.com/goerz/xmgrace_parser), written by [Michael Goerz](http://michaelgoerz.net)
