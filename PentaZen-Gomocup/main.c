/*
 *  PentaZen Gomoku/Renju Engine
 *                    _______
 *                   / _____/
 *  developed by    / /______  ________
 *                 /____  / / / / __  /
 *                _____/ / /_/ / / / /
 *  2019.04      /______/_____/_/ /_/
 *
 * main.c - main function
 */

#include "gomocup.h"

// global variables
board_t Bd;
search_t Srh;

// main function
int main()
{
#if SEARCH_TEST
	search_test(&Bd, &Srh);
#else
	gomocup_loop(&Bd, &Srh);
#endif
	return 0;
}
