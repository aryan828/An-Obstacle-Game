#ifndef GAME_H
#define GAME_H

#include <curses.h>

#include "screen.h"

class Game {
public:
    Game();
    ~Game() { endwin(); }
    void launchGame();
};

#endif