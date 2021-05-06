#include "../include/screen.h"

void GameScreen::move_my_vehicle(std::promise<void> &&postman) {
    int playerRow = _row-2;
    int playerCol = _col/2;
    changeDisplayMatrix(playerRow, playerCol, 2);
    _board->update_cell(playerRow, playerCol, 2, -2, -2);

    // notifying obstacle generation thread to create obstacle
    if(!vehicle_created) {
        std::unique_lock<std::mutex> locker(_mutex);
        vehicle_created = true;
        _cv.notify_all();
    }

    int pressed_key;
    while(game_should_go_on) {
        pressed_key = getch();
        switch (pressed_key) {
            case KEY_UP:
                if(playerRow > 0) {
                    changeDisplayMatrix(playerRow, playerCol, 0);
                    playerRow -= 1;
                    if(checkPlayerCollision(playerRow, playerCol)) {
                        stopGame();
                        break;
                    }
                    changeDisplayMatrix(playerRow, playerCol, 2);
                    int prev_row = playerRow + 1;
                    int prev_col = playerCol;
                    _board->update_cell(playerRow, playerCol, 2, prev_row, prev_col);
                }
                break;
            case KEY_LEFT:
                if(playerCol > 0) {
                    changeDisplayMatrix(playerRow, playerCol, 0);
                    playerCol -= 1;
                    if(checkPlayerCollision(playerRow, playerCol)) {
                        stopGame();
                        break;
                    }
                    changeDisplayMatrix(playerRow, playerCol, 2);
                    int prev_row = playerRow;
                    int prev_col = playerCol + 1;
                    _board->update_cell(playerRow, playerCol, 2, prev_row, prev_col);
                }
                break;
            case KEY_DOWN:
                if(playerRow < _row - 1) {
                    changeDisplayMatrix(playerRow, playerCol, 0);
                    playerRow += 1;
                    if(checkPlayerCollision(playerRow, playerCol)) {
                        stopGame();
                        break;
                    }
                    changeDisplayMatrix(playerRow, playerCol, 2);
                    int prev_row = playerRow - 1;
                    int prev_col = playerCol;
                    _board->update_cell(playerRow, playerCol, 2, prev_row, prev_col);
                }
                break;
            case KEY_RIGHT:
                if(playerCol < _col - 1) {
                    changeDisplayMatrix(playerRow, playerCol, 0);
                    playerCol += 1;
                    if(checkPlayerCollision(playerRow, playerCol)) {
                        stopGame();
                        break;
                    }
                    changeDisplayMatrix(playerRow, playerCol, 2);
                    int prev_row = playerRow;
                    int prev_col = playerCol - 1;
                    _board->update_cell(playerRow, playerCol, 2, prev_row, prev_col);
                }
                break;
            default:
                break;
        }
    }
    postman.set_value();
}

void GameScreen::generate_obstacle() {
    while(!vehicle_created) {
        // wait till vehicle isn't created
        std::unique_lock<std::mutex> locker(_mutex);
        _cv.wait(locker);
    }

    while(game_should_go_on) {
        int bound = getGapIndex();
//            int row = 0;

        Obstacle obstacle;
        obstacle.row = 0;

//            std::vector<int> cols;
        for(int i=0; i<bound; i++) {
            changeDisplayMatrix(obstacle.row, i, 1);
//                cols.push_back(i);
            obstacle.cols.emplace_back(i);
        }
        for(int i=bound + getObstacleGap(); i<_col; i++) {
            changeDisplayMatrix(obstacle.row, i, 1);
//                cols.push_back(i);
            obstacle.cols.emplace_back(i);
        }

//            _board->update_cell(row, cols, 1);
        _board->update_cell(obstacle);

        //moving downward
        for(int r = 0; r < _row; r++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(getObstacleDelay())); // waiting time of updating obstacle row
            for(int col : obstacle.cols) {
                changeDisplayMatrix(r, col, 0); // changing current row val to 0
                if(r < _row - 1) { // shouldn't do this if this is last row.
                    if(checkObstacleCollision(r+1, col)) {
                        stopGame();
                        break;
                    }
                    changeDisplayMatrix(r+1, col, 1); // Taking obstacle in below row
                }
            }
            obstacle.row++;
            _board->update_cell(obstacle);
            if(!game_should_go_on) break;
        }
        if(!game_should_go_on) break;
        score++; // each obstacle goes out of the board, score increases.
    }
    //sender.set_value();
}

int GameScreen::getGapIndex() {
    int mid = _col/2;
    int index;

    if(score % 2 == 0) {
        // generate gap on left side
        srand(time(0));
        index = rand() % mid;
    } else {
        // gap on right side
        // start from mid point to (_col - # of gap)
        srand(mid);
        index =  rand() % (_col - getObstacleGap()) ;
    }
    return index;
}

void GameScreen::changeDisplayMatrix(int b_row, int b_col, int val) {
    std::lock_guard<std::mutex> locker(_mutex);
    _inner_board[b_row][b_col] = val;
}

int GameScreen::getMatrixCell(int row, int col) {
    return _inner_board[row][col];
}

void GameScreen::postGameOver() const {
    clear();
    wrefresh(_win);
    attron(A_STANDOUT);
    printw("GAME OVER!\n");
    attroff(A_STANDOUT);
    attron(A_UNDERLINE);
    printw("Your score: %d\n", score);
    attroff(A_UNDERLINE);
    attron(A_DIM);
    printw("Press SPACE to exit!");
    attroff(A_DIM);
    int inp = getch();
    while(inp != 32) {
        inp = getch();
    }
}

bool GameScreen::checkPlayerCollision(int row, int col) {
    if(getMatrixCell(row, col) == 1) { // vehicle found obstacle in this position
        return true;
    }
    return false;
}

bool GameScreen::checkObstacleCollision(int row, int col) {
    if(getMatrixCell(row, col) == 2) { // obstacle found vehicle in this position
        return true;
    }
    return false;
}

void GameScreen::stopGame() {
    std::lock_guard<std::mutex> locker(_mutex);
    game_should_go_on = false;
}

int GameScreen::getObstacleDelay() {
    if(score <= 5) {
        obstacle_delay = 200;
    } else if(score > 5 && score <= 10) {
        obstacle_delay = 160;
    } else if(score > 10 && score <= 20) {
        obstacle_delay = 120;
    } else if(score > 20 && score <= 30) {
        obstacle_delay = 80;
    } else {
        obstacle_delay = 50;
    }
    return obstacle_delay;
}

int GameScreen::getObstacleGap() {
    if(score <= 10) {
        obstacle_max_gap = 6;
    } else if(score > 10 && score <= 20) {
        obstacle_max_gap = 5;
    } else if(score > 20 && score <= 30) {
        obstacle_max_gap = 4;
    } else if(score > 30 && score <= 40) {
        obstacle_max_gap = 3;
    } else  {
        obstacle_max_gap = 2;
    }
    return obstacle_max_gap;
}

GameScreen::GameScreen(int r, int c, WINDOW *win, std::unique_ptr<Board_Generator> board) {
    _row = r-2;
    _col = c-2;
    _win = win;
    _board = std::move(board);
//        box(_win, 0, 0);
    wrefresh(_win);
}

void GameScreen::initScreen() {
    // initialize inner vector
    std::vector<int> inner;
    for (int i = 0; i < _row; i++) {
        inner.clear();
        for (int j = 0; j < _col; j++) {
            inner.push_back(0);
        }
        _inner_board.push_back(inner);
    }
}

void GameScreen::launch_game() {
    _board->draw_board();

    // generating and moving obstacle continuously
    std::future<void> obstacle_thread = std::async(std::launch::async, &GameScreen::generate_obstacle, this);

    // creating my vehicle thread
    std::promise<void> playerThreadPromise;
    std::future<void> playerThreadFuture = playerThreadPromise.get_future();
    std::thread vehicle_thread = std::thread(&GameScreen::move_my_vehicle, this, std::move(playerThreadPromise));

    // returns once game is over
    playerThreadFuture.wait();
    vehicle_thread.join();
    obstacle_thread.wait();

    // display text after game is over
    postGameOver();
}

void GameScreen::print_inner_board() {
    for(int row=0; row<_row; row++) {
        for(int col=0; col<_col; col++) {
            mvwprintw(_win, row+30, col, "%d", _inner_board[row][col]);
        }
        printw("\n");
    }
    wrefresh(_win);
}

void WelcomeScreen::printMenu() {
    ITEM **options;
    int choice;
    MENU *mainMenu;
    int nChoices = messages.size();
    ITEM *curItem;
    options = (ITEM **) calloc(nChoices+1, sizeof(ITEM *));
    for(int i = 0; i<nChoices; i++) {
        options[i] = new_item(messages[i], "");
    }
    options[nChoices] = (ITEM *)nullptr;
    mainMenu = new_menu((ITEM **)options);
    set_menu_win(mainMenu, screen);
    set_menu_sub(mainMenu, derwin(screen, 5, 25, 14, 38));
    refresh();
    post_menu(mainMenu);
    mvwaddch(screen, 2, 0, ACS_LTEE);
    mvwhline(screen, 2, 1, ACS_HLINE, 85);
    mvwaddch(screen, 2, 86, ACS_RTEE);
    wattron(screen, A_BOLD);
    mvwprintw(screen, 1, 38, "MAIN MENU");
    wattroff(screen, A_BOLD);
    wrefresh(screen);
    while ((choice=getch()) != KEY_F(1)) {
        switch (choice) {
            case KEY_DOWN:
                menu_driver(mainMenu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(mainMenu, REQ_UP_ITEM);
                break;
            case 10:
                curItem = current_item(mainMenu);
                if(curItem == options[0]) {
                    WINDOW *gameScreen = newwin(31, 85, xPos+1, yPos+1);
                    std::unique_ptr<Board_Generator> board(new Board_Generator(board_length, board_width, gameScreen));

                    // Init Game
                    std::unique_ptr<GameScreen> game(new GameScreen(board_length, board_width, gameScreen, std::move(board)));
                    game->initScreen(); // init inner matrix or board
                    game->launch_game();
                } else if( curItem == options[1]) {
                    PlayerIPScreen playerIpScreen;
                    playerIpScreen.initScreen();
                } else { return; }
                break;
        }
        wrefresh(screen);
    }
    for(int i = 0; i<=nChoices; i++) free_item(options[i]);
    free_menu(mainMenu);
}
