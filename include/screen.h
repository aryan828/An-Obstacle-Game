#ifndef SCREEN_H
#define SCREEN_H

#include <menu.h>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <pthread.h>
#include <future>
#include <form.h>
#include <fstream>
#include <iostream>

#include "obstacle.h"
#include "board_generator.h"
#include "player.h"
#include "FileManager.h"

class Screen {
public:
    WINDOW *screen;
    virtual void initScreen() = 0;
};

class GameScreen: Screen {
private:
    std::vector<std::vector<int>> display_matrix;
    int _row;
    int _col;
    std::unique_ptr<Board_Generator> _board;
    std::mutex m;
    std::condition_variable _cv;
    bool game_should_go_on = true;
    bool player_created = false;
    Player player;
    void move_my_player();
    void generate_obstacle();
    void changeDisplayMatrix(int b_row, int b_col, int val);
    int getMatrixCell(int row, int col);
    bool checkPlayerCollision(int row, int col);
    bool checkObstacleCollision(int row, int col);
    void stopGame();

public:
    GameScreen(int r, int c, WINDOW *win, std::unique_ptr<Board_Generator> board, Player player);
    ~GameScreen() { delwin(screen); }
    void initScreen();
    void launch_game();
};

class PlayerIPScreen: Screen {
    int xPos = 0;
    int yPos = 0;
    int board_length = 33;
    int board_width = 87;
    Player player;

public:
    PlayerIPScreen();
    ~PlayerIPScreen() { delwin(screen); }
    void initScreen() override;
};

class WelcomeScreen: Screen {
    int xPos;
    int yPos;
    std::vector<char*> messages {"NEW GAME", "SCORES", "EXIT"};

public:
    WelcomeScreen() {
        xPos = 1;
        yPos = 1;
        screen = newwin(33, 87, yPos, xPos);
    }

    ~WelcomeScreen() { delwin(screen); }

    void initScreen() override;

    void printMenu();

};

class EndScreen: Screen {
    int xPos;
    int yPos;
    Player player;

public:
    EndScreen(Player player);

    ~EndScreen() { delwin(screen); }

    void initScreen() override;
};

class ScoreScreen: Screen {
    int xPos;
    int yPos;

public:
    ScoreScreen();

    ~ScoreScreen() { delwin(screen); }

    void initScreen() override;

};

#endif //SCREEN_H