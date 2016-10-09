# disgrace

Disgrace will be a cross-platform, qt5-based plotting tool. I have been using xmgrace since I started doing research, and many of its features 
cannot be found in any other plotting tool. My idea is to rebuild an xmgrace-like tool with a modern graphic toolkit (qt5) and with a more 
modern software architecture (taking into account modularity, plugins, _etc_). I am aware that a qt version of xmgrace is being developed 
(see [here](https://sourceforge.net/projects/qtgrace/) for more details). While this huge effort is surely commendable, I believe that a 
complete rewrite is necessary. 

For now, this document will contain the list of features that I would like to implement.

## Features
* Undo/redo
* Disgrace should be able to read in sets from files passed as command-line arguments
* Disgrace files should also contain the data sets
* Disgrace should be able to parse xmgrace .agr files. Maybe also write them?
* Heat maps should be supported
* It should be possible to operate on data sets (interpolations, regressions and most of the operations available in the _data_ xmgrace menu)

## Optional features
* 3D surface plotting
