/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun.
 */

#ifndef __DISTANCE_H__
#define __DISTANCE_H__

#include "public.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NEI_MAX     16

extern Metric Dist[BD_SIZE][BD_SIZE];
extern Pos Nei[BD_SIZE][NEI_MAX];
extern u8 NeiInd[BD_SIZE];

void generate_dist_nei_table();

#ifdef __cplusplus
}
#endif

#endif
