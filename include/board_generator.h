#ifndef BOARD_GENERATOR_H
#define BOARD_GENERATOR_H

#include <chrono>
#include <mutex>
#include <curses.h>

#include "obstacle.h"

class Board_Generator {
    unsigned int _length;
    unsigned int _width;
    WINDOW *_win;

public:
    Board_Generator(int &l, int &w, WINDOW *win);
    ~Board_Generator() { delwin(_win); }
    void draw_board();
    void update_cell(int row, int col, int val, int prev_row, int prev_col);
    void update_cell(Obstacle obstacle);
};

#endif //BOARD_GENERATOR_H
