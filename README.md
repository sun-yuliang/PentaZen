# PentaZen

PentaZen is a strong gomoku/renju playing engine supporting [Gomocup protocol](http://petr.lastovicka.sweb.cz/protocl2en.htm). The latest release supports some of [Yixin protocol](https://github.com/accreator/Yixin-protocol).

In [Gomocup](https://gomocup.org/) 2019, PentaZen 0.1.0 ranks 19th in freestyle league, 16th in fastgame league, 12th in standard league and 8th in renju league.

In [Gomocup](https://gomocup.org/) 2020, PentaZen 0.3.2 ranks 4th in freestyle and fastgame league, 3rd in standard and renju league.

The latest commit in master branch is the source code of PentaZen 0.2.1. The newer versions are much stronger, i.e. almost 100% beats PentaZen 0.2.1.

## Getting Started

### Prerequisites

You need to download [Piskvork gomoku manager](https://sourceforge.net/projects/piskvork/files/piskvork.zip/download) to run PentaZen engine.

### Compiling

The source codes are compiled using mingw-w64 on Windows 10.

## Running the Engine

In Piskvork, you can choose pbrain-PentaZen.exe as the engine. The detailed instructions can be found in the readme file of Piskvork.

## Known Defects (0.2.1)

* Cannot recognize recursively defined fouls.
* Occasionally return false victory evaluation.

## Author

* **Sun**

## License

See the license file for details.
