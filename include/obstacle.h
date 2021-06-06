#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <vector>

using std::vector;

class Obstacle {
public:
    const char *obstacleSymbol = "#";
    std::vector<int> cols;
    int row;
    static int getGapIndex(int, int);

    static int getObstacleGap(int);

    static int getObstacleDelay(int);
};

#endif //OBSTACLE_H
