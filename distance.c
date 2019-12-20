/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun.
 */

#include "distance.h"

// Distance tables.
Metric Dist[BD_SIZE][BD_SIZE];

// Neighbor tables.
Pos Nei[BD_SIZE][NEI_MAX];
u8 NeiInd[BD_SIZE];

// Euclidean distance sqr(x^2 + y^2).
static Metric euclidean_distance(const Pos pos1, const Pos pos2)
{
    double x = abs(pos1 / BD_SIDE - pos2 / BD_SIDE);
    double y = abs(pos1 % BD_SIDE - pos2 % BD_SIDE);
    return (Metric)sqrt(x * x + y * y) + 1;
}

// Radius distance (square radius).
static Metric radius_distance(const Pos pos1, const Pos pos2)
{
    Metric x = abs(pos1 / BD_SIDE - pos2 / BD_SIDE);
    Metric y = abs(pos1 % BD_SIDE - pos2 % BD_SIDE);
    return MyMax(x, y);
}

// Distance used to generate neighbor positions.
static Metric dist(const Pos pos1, const Pos pos2)
{
    Metric x = abs(pos1 / BD_SIDE - pos2 / BD_SIDE);
    Metric y = abs(pos1 % BD_SIDE - pos2 % BD_SIDE);

    return x == y || x == 0 || y == 0 ? radius_distance(pos1, pos2)
                                      : euclidean_distance(pos1, pos2);
}

// Generate distance and neighbor tables.
void generate_dist_nei_table()
{
    Pos i, j;
    u8 cnt;

    for (i = 0; i < BD_SIZE; i++)
    {
        cnt = 0;
        for (j = 0; j < BD_SIZE; j++)
        {
            Dist[i][j] = dist(i, j);
            if (0 < Dist[i][j] && Dist[i][j] <= 2)
                Nei[i][cnt++] = j;
        }
        NeiInd[i] = cnt;
    }
}
