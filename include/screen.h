#ifndef SCREEN_H
#define SCREEN_H

#include <menu.h>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <pthread.h>
#include <future>

#include "obstacle.h"
#include "board_generator.h"

class Screen {
public:
    WINDOW *screen;
    virtual void initScreen() = 0;
};

class GameScreen: Screen {
private:
    std::vector<std::vector<int> > _inner_board;
    int _row;
    int _col;
    int score = 0;
    WINDOW *_win;
    int obstacle_delay;  // increases to get higher speed
    int obstacle_max_gap; // gap for vehicle to pass through
    std::unique_ptr<Board_Generator> _board;
    std::mutex _mutex;
    std::condition_variable _cv;
    bool game_should_go_on = true;
    bool vehicle_created = false; // obstacle generation should wait until player/vehicle is created... this var controls that
    void move_my_vehicle(std::promise<void> &&postman);
    void generate_obstacle();
    int getGapIndex();
    void changeDisplayMatrix(int b_row, int b_col, int val);
    int getMatrixCell(int row, int col);
    void postGameOver() const;
    bool checkPlayerCollision(int row, int col);
    bool checkObstacleCollision(int row, int col);
    void stopGame();
    int getObstacleDelay();
    int getObstacleGap();

public:
    GameScreen(int r, int c, WINDOW *win, std::unique_ptr<Board_Generator> board);
    ~GameScreen() { delwin(_win); }
    void initScreen();
    void launch_game();
    void print_inner_board(); // dummy method for test
};

class PlayerIPScreen: Screen {
    int xPos = 0;
    int yPos = 0;
public:
    PlayerIPScreen() {
        xPos = 1;
        yPos = 1;
        screen = newwin(33, 87, yPos, xPos);
        box(screen, 0, 0);
    }

    void initScreen() override {
        refresh();
        wprintw(screen, "PlayerIPScreen Works");
        wrefresh(screen);
        getch();
    }
};

class WelcomeScreen: Screen {
    int xPos;
    int yPos;
    int board_length = 33;
    int board_width = 87;
    std::vector<char*> messages {"NEW GAME", "SCORES", "EXIT"};

public:
    WelcomeScreen() {
        xPos = 1;
        yPos = 1;
        screen = newwin(33, 87, yPos, xPos);
    }
    void initScreen() override {
        box(screen, 0, 0);
        refresh();
        wrefresh(screen);
        printMenu();
    }

    void printMenu();

};

#endif //SCREEN_H
