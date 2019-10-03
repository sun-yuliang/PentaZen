# Notice
Pentazen2019 is PentaZen 0.1.0. The newer versions are under development, which differ a lot and are much stronger than PentaZen2019.

# PentaZen2019

PentaZen2019 is a gomoku/renju playing engine supporting [Gomocup protocol](http://petr.lastovicka.sweb.cz/protocl2en.htm). It is based on heuristic alpha-beta search accelerated by transposition table and table look-up. It can search 10-14 ply and calculate some VCF/VCT of 20-30 ply in several seconds during midgames. In [Gomocup 2019](https://gomocup.org/results/gomocup-result-2019/), PentaZen ranks 19th in freestyle league and 16th in fastgame league.

## Getting Started

### Environment

PentaZen2019 is capatible with 64-bit Windows 7 and 64-bit Windows 10.

### Download

Before running PentaZen2019, you need to [download Piskvork gomoku manager](https://sourceforge.net/projects/piskvork/files/piskvork.zip/download) and [download PentaZen2019 engine](https://github.com/sun-yuliang/PentaZen-2019/releases/download/v0.1.0/pbrain-PentaZen2019_64.zip).

## Running the Program

In Piskvork, you can choose pbrain-PentaZen2019.exe as the engine. The detailed infomation can be found in the Readme file of Piskvork.

## Defects

Note that some of the following defects are improved in newer versions.

 * Use not exhuastive pattern tables.
 * Cannot recognize recursively defined fouls.
 * Prune too many moves, resulting in inaccurate evaluation.

## Author

* **Sun**

## License

This project is licensed under the GPL License - see the [LICENSE](https://github.com/sun-yuliang/PentaZen-2019/blob/master/LICENSE) file for details.
