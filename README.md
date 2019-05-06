# PentaZen-Gomocup
PentaZen is a gomoku/renju engine supporting Gomocup protocol. Please compile first and download Piskvork gomoku manager from https://gomocup.org/download-gomocup-manager to run it.

## Overview
PentaZen engine is written in C. It is compatible with Windows7 and Windows10. It supports freestyle(five or more stones win), standard(exactly five stones win) and renju rules. For freestyle and standard rules, the board size should be 15 * 15 or 20 * 20. For renju rule, the board size should be 15 * 15. In Gomocup 2019, PentaZen ranks 19th in freestyle league and 16th in fastgame league.

## Algorithm and Performance
PentaZen uses classic searching techniques. The basic algorithm is minimax search with pruning accelerated by hash table. Under Gomocup fastgame time limit(5s per move, 120s per match), PentaZen can finish 30-ply VCF search, 20-ply VCT search and 10-ply to 14-ply pruned minimax search per step.

# PentaZen-Gomocup
PentaZen是支持Gomocup协议的五子棋/连珠引擎。在工程里编译之后用Piskvork挂载exe文件即可运行。Piskvork下载地址：https://gomocup.org/download-gomocup-manager

## 简介
PentaZen引擎是用C语言编写的，可以在Windows7和Windows10环境下运行。PentaZen支持无禁手，无禁手五连胜和连珠规则（不支持打点和交换）。两种无禁手规则下，棋盘大小可以是15 * 15或20 * 20，连珠规则下棋盘大小只能是15 * 15。在2019年Gomocup世界五子棋AI锦标赛中，PentaZen慢棋组排名第19位，快棋组排名第16位。

## 算法性能
PentaZen采用传统搜索技术，即带剪枝的极大极小搜索和散列表加速。在每步5秒，每局120秒的时限下，PentaZen每步都可以完成30步VCF算杀，20步VCT算杀和10-14步带剪枝的极大极小搜索。
