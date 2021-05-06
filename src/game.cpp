#include "../include/game.h"

Game::Game() {
    startGame = true;
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
}

void Game::launchGame() const {
    if(!startGame) return;
    WelcomeScreen welcomeScreen;
    welcomeScreen.initScreen();
}
