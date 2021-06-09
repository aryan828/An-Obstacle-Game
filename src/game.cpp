#include "../include/game.h"

Game::Game() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
}

void Game::launchGame() {
    WelcomeScreen welcomeScreen;
    welcomeScreen.initScreen();
}
