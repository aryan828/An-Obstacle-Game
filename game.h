#ifndef GAME_H
#define GAME_H

#include <curses.h>

#include "screen.h"

class Game {
    bool startGame;
public:
    Game() {
        startGame = true;
        initscr();
        noecho();
        cbreak();
        keypad(stdscr, TRUE);
    }

    ~Game() { endwin(); }

    void launchGame() const {
        if(!startGame) return;
        WelcomeScreen welcomeScreen;
        welcomeScreen.initScreen();
    }
};

#endif