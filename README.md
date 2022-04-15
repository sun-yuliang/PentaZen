# PentaZen

PentaZen is a strong gomoku/renju playing engine supporting [Gomocup protocol](http://petr.lastovicka.sweb.cz/protocl2en.htm). The latest release supports some of [Yixin protocol](https://github.com/accreator/Yixin-protocol).

The latest commit in master branch is the source code of PentaZen 0.4.18, the version for Gomocup 2021. The outdated versions can be found in other branches.

PentaZen 0.4.18 is based on pure alpha-beta search framework. Starting from PentaZen 0.5.0, neural networks are involved.

## Compiling

The source codes can be compiled using mingw-w64 on Windows 10. To compile the main program, you need to generate pattern header files for different rules first by compiling the code in src/pattern folder and running the binary.

## Running the Engine

The engine supports Gomocup protocol and part of Yixin board commands. You can run the engine through command line or by any GUI supporting these protocols.

## Known Defects

* Cannot recognize recursively defined fouls more than 2 ply.

## Author

* **Sun Yuliang**
