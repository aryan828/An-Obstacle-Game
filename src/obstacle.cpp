//
// Created by aryan on 04/05/21.
//

#include <ctime>
#include "../include/obstacle.h"

int Obstacle::getGapIndex(int col, int score) {
    int mid = col/2;
    int index;

    if(score % 2 == 0) {
        // generate gap on left side
        srand(time(0));
        index = rand() % mid;
    } else {
        // gap on right side
        // start from mid point to (_col - # of gap)
        srand(mid);
        index =  rand() % (col - Obstacle::getObstacleGap(score)) ;
    }
    return index;
}

int Obstacle::getObstacleGap(int score) {
    if(score <= 10) {
        return 6;
    } else if(score > 10 && score <= 20) {
        return 5;
    } else if(score > 20 && score <= 30) {
        return 4;
    } else if(score > 30 && score <= 40) {
        return 3;
    } else  {
        return 2;
    }
}

int Obstacle::getObstacleDelay(int score) {
    if(score <= 5) {
        return 200;
    } else if(score > 5 && score <= 10) {
        return 160;
    } else if(score > 10 && score <= 20) {
        return 120;
    } else if(score > 20 && score <= 30) {
        return 80;
    } else {
        return 50;
    }
}