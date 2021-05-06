#include "../include/board_generator.h"

Board_Generator::Board_Generator(int &l, int &w, WINDOW *win) {
    _length = l;
    _width = w;
    _win = win;
}

void Board_Generator::draw_board() {
    refresh();
    for(int row = 0; row < _length; row++) {
        for(int col = 1; col <= _width-2; col++) {
            mvwprintw(_win, row, col, " ");
        }
    }
    wrefresh(_win);
}

void Board_Generator::update_cell(int row, int col, int val, int prev_row, int prev_col) {
    mvwprintw(_win, row+1, col+1, "*");
    mvwprintw(_win, prev_row+1, prev_col+1, " ");
    wrefresh(_win);
}

void Board_Generator::update_cell(Obstacle obstacle) {
    for(int i=0; i<obstacle.cols.size(); i++) {
        if(obstacle.row > 0) mvwprintw(_win, obstacle.row, obstacle.cols[i]+1, " ");
        if(obstacle.row+1 < _length-1) mvwprintw(_win, obstacle.row+1, obstacle.cols[i]+1, obstacle.obstacleSymbol);
    }
    wrefresh(_win);
}
