# PentaZen-Gomocup
PentaZen is a gomoku/renju engine supporting gomocup protocol. To run the engine, please download Piskvork gomoku manager from https://gomocup.org/download-gomocup-manager/.
PentaZen是一个使用gomocup协议的五子棋/连珠引擎。请从https://gomocup.org/download-gomocup-manager/下载Piskvork来运行PentaZen。

## Overview
PentaZen engine is written in C. It is compatible with Windows7 and Windows10. It supports freestyle(five or more stones win), standard(exactly five stones win) and renju rules. For freestyle and standard rules, the board size should be 15 * 15 or 20 * 20. For renju rule, the board size should be 15 * 15.
PentaZen引擎是用C语言编写的，可以在Windows7和Windows10环境下运行。PentaZen支持无禁手，无禁手五连胜和连珠规则（不支持打点和交换）。两种无禁手规则下，棋盘大小可以是15 * 15或20 * 20，连珠规则下棋盘大小只能是15 * 15。

## Algorithm and Performance
PentaZen uses classic searching techniques. The basic algorithm is minimax search with pruning accelerated by hash table and VCF and VCT search. Under gomocup fastgame time limit(5s per move, 120s per match), PentaZen can finish 5-ply to 7-ply pruned minimax search and 10-ply VCT search of one color per move.
PentaZen采用传统搜索技术，即带剪枝的极大极小搜索，以及VCF和VCT搜索。极大极小搜索使用了散列表加速。在每步5秒，每局120秒的时限下，PentaZen中局每步可以保证单色5-7步极大极小搜索和单色10步VCT搜索。
