#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <vector>

using std::vector;

class Obstacle {
public:
    const char *obstacleSymbol = "#";
    std::vector<int> cols;
    int row;
};

#endif //OBSTACLE_H
