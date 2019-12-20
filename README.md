# PentaZen

PentaZen is a gomoku/renju playing engine supporting [Gomocup protocol](http://petr.lastovicka.sweb.cz/protocl2en.htm). It is based on heuristic alpha-beta search with some other pruning techniques accelerated by table look-up. In [Gomocup 2019](https://gomocup.org/results/gomocup-result-2019/), PentaZen v0.1.0 ranks 19th in the freestyle league and 16th in the fastgame league.

## Getting Started

### Prerequisites

You need to download [Piskvork gomoku manager](https://sourceforge.net/projects/piskvork/files/piskvork.zip/download) to run PentaZen engine.

### Compiling

The source codes are compiled using mingw-w64 on Windows 10.

## Running the Engine

In Piskvork, you can choose pbrain-PentaZen.exe as the engine. The detailed instructions can be found in the readme file of Piskvork.

## Known Defects

* Cannot recognize recursively defined fouls.
* Occasionally return false victory evaluation.

## Author

* **Sun**

## License

This project is licensed under the GPL License - see the license file for details.
