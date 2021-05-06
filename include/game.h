#ifndef GAME_H
#define GAME_H

#include <curses.h>

#include "screen.h"

class Game {
    bool startGame;
public:
    Game();
    ~Game() { endwin(); }
    void launchGame() const;
};

#endif