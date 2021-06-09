#include "../include/screen.h"

void GameScreen::move_my_player() {
    int playerRow = _row-2;
    int playerCol = _col/2;
    changeDisplayMatrix(playerRow, playerCol, 2);
    _board->update_cell(playerRow, playerCol, 2, -2, -2);

    if(!player_created) {
        std::unique_lock<std::mutex> locker(m);
        player_created = true;
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
}

void GameScreen::generate_obstacle() {
    while(!player_created) {
        // wait till vehicle isn't created
        std::unique_lock<std::mutex> locker(m);
        _cv.wait(locker);
    }

    while(game_should_go_on) {
        int bound = Obstacle::getGapIndex(_row, player.score);
        Obstacle obstacle;
        obstacle.row = 0;

        for(int i=0; i<bound; i++) {
            changeDisplayMatrix(obstacle.row, i, 1);
            obstacle.cols.emplace_back(i);
        }

        for(int i=bound + Obstacle::getObstacleGap(player.score); i<_col; i++) {
            changeDisplayMatrix(obstacle.row, i, 1);
            obstacle.cols.emplace_back(i);
        }

        _board->update_cell(obstacle);

        //moving downward
        for(int r = 0; r < _row; r++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(Obstacle::getObstacleDelay(player.score)));
            for(int col : obstacle.cols) {
                changeDisplayMatrix(r, col, 0);
                if(r < _row - 1) {
                    if(checkObstacleCollision(r+1, col)) {
                        stopGame();
                        break;
                    }
                    changeDisplayMatrix(r+1, col, 1);
                }
            }
            obstacle.row++;
            _board->update_cell(obstacle);
            if(!game_should_go_on) break;
        }
        if(!game_should_go_on) break;
        player.score++;
    }
}

void GameScreen::changeDisplayMatrix(int b_row, int b_col, int val) {
    std::lock_guard<std::mutex> locker(m);
    display_matrix[b_row][b_col] = val;
}

int GameScreen::getMatrixCell(int row, int col) {
    return display_matrix[row][col];
}

bool GameScreen::checkPlayerCollision(int row, int col) {
    if(getMatrixCell(row, col) == 1) { // PLAYER found OBSTACLE at this position
        return true;
    }
    return false;
}

bool GameScreen::checkObstacleCollision(int row, int col) {
    if(getMatrixCell(row, col) == 2) { // OBSTACLE found PLAYER at this position
        return true;
    }
    return false;
}

void GameScreen::stopGame() {
    std::lock_guard<std::mutex> locker(m);
    game_should_go_on = false;
}

GameScreen::GameScreen(int r, int c, WINDOW *win, std::unique_ptr<Board_Generator> board, Player player) {
    _row = r-2;
    _col = c-2;
    screen = win;
    _board = std::move(board);
    wrefresh(screen);
    this->player = player;
}

void GameScreen::initScreen() {
    // initialize inner vector
    std::vector<int> inner(_col, 0);
    for (int i = 0; i < _row; i++) {
        display_matrix.push_back(inner);
    }
}

void GameScreen::launch_game() {
    _board->draw_board();

    // generating and moving obstacle continuously
    std::future<void> obstacle_thread = std::async(std::launch::async, &GameScreen::generate_obstacle, this);

    std::future<void> player_thread = std::async(std::launch::async, &GameScreen::move_my_player, this);

    player_thread.wait();
    obstacle_thread.wait();

    EndScreen endScreen(player);
    endScreen.initScreen();
}

void WelcomeScreen::initScreen() {
    box(screen, 0, 0);
    refresh();
    wrefresh(screen);
    printMenu();
}

void WelcomeScreen::printMenu() {
    ITEM **options;
    int choice;
    MENU *mainMenu;
    int nChoices = (int)messages.size();
    ITEM *curItem;
    options = (ITEM **) calloc(nChoices+1, sizeof(ITEM *));
    for(int i = 0; i<nChoices; i++) {
        options[i] = new_item(messages[i], "");
    }
    options[nChoices] = (ITEM *)nullptr;
    mainMenu = new_menu(options);
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
    while ((choice=getch())) {
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
                    PlayerIPScreen playerIpScreen;
                    playerIpScreen.initScreen();
                    clear();
                    wrefresh(screen);
                } else if( curItem == options[1]) {
                    ScoreScreen scoreScreen;
                    scoreScreen.initScreen();
                    clear();
                    wrefresh(screen);
                } else { return; }
                break;
            default:
                break;
        }
        box(screen, 0, 0);
        wrefresh(screen);
    }
    for(int i = 0; i<=nChoices; i++) free_item(options[i]);
    free_menu(mainMenu);
}

PlayerIPScreen::PlayerIPScreen() {
    xPos = 1;
    yPos = 1;
    screen = newwin(33, 87, yPos, xPos);
    box(screen, 0, 0);
    wrefresh(screen);
    keypad(screen, TRUE);
}

void PlayerIPScreen::initScreen() {
    mvwprintw(screen, 11, 30, "ENTER PLAYER NAME: ");
    wrefresh(screen);
    int count = 0;
    int choice;
    while ((choice = getch()) != 10) {
        try {
            if((choice >= 48 && choice <= 57) || (choice >= 97 && choice <= 122) || (choice >= 65 && choice <= 90)) {
                if(count>10) throw "No more characters allowed!";
                player.player_name[count] = (char) choice;
                wprintw(screen, "%c", choice);
                wrefresh(screen);
                count++;
            } else if(choice == KEY_BACKSPACE) {
                if (count > 0) {
                    mvwprintw(screen, 11, 48 + count, " ");
                    wmove(screen, 11, 48 + count);
                    wrefresh(screen);
                    player.player_name[count] = ' ';
                    count--;
                }
            } else throw "Character not allowed!";
        } catch (const char* error) {
            mvwprintw(screen, 13, 30, "%s", error);
            wmove(screen, 11, 49 + count);
            wrefresh(screen);
        }
    }
    WINDOW *gameScreen = newwin(31, 85, xPos+1, yPos+1);
    refresh();
//                    box(gameScreen, 0, 0);
    wrefresh(gameScreen);
    std::unique_ptr<Board_Generator> board(new Board_Generator(board_length, board_width, gameScreen));

    // Init Game
    std::unique_ptr<GameScreen> game(new GameScreen(board_length, board_width, gameScreen, std::move(board), player));
    game->initScreen(); // init inner matrix or board
    game->launch_game();
}

EndScreen::EndScreen(Player player) {
    xPos = 1;
    yPos = 1;
    screen = newwin(33, 87, yPos, xPos);
    this->player = player;
    box(screen, 0, 0);
    wrefresh(screen);
}

void EndScreen::initScreen() {
//    clear();
    refresh();
    wrefresh(screen);
    wattron(screen, A_STANDOUT);
    mvwprintw(screen, 14, 38, "GAME OVER!");
    wattroff(screen, A_STANDOUT);
    wattron(screen, A_UNDERLINE);
    mvwprintw(screen, 16, 38, "%s", player.player_name);
    mvwprintw(screen, 17, 36, "Your score: %d", player.score);
    wattroff(screen, A_UNDERLINE);
    wattron(screen, A_DIM);
    mvwprintw(screen, 19, 32, "Press SPACE to exit!");
    wattroff(screen, A_DIM);
    wrefresh(screen);
    int inp = getch();
    while(inp != 32) {
        inp = getch();
    }
    clear();
    FileManager::writeScore(player);
}

ScoreScreen::ScoreScreen() {
    xPos = 1;
    yPos = 1;
    screen = newwin(33, 87, yPos, xPos);
    box(screen, 0, 0);
    wrefresh(screen);
}

void ScoreScreen::initScreen() {
    mvwaddch(screen, 2, 0, ACS_LTEE);
    mvwhline(screen, 2, 1, ACS_HLINE, 85);
    mvwaddch(screen, 2, 86, ACS_RTEE);
    wattron(screen, A_BOLD);
    mvwprintw(screen, 1, 38, "LAST SCORES");
    mvwprintw(screen, 3, 35, "NAME");
    mvwprintw(screen, 3, 46, "SCORE");
    wattroff(screen, A_BOLD);
    wrefresh(screen);
    FileManager::readScores(screen);
    mvwprintw(screen, 25, 30, "Press any key to go back!");
    wrefresh(screen);
    getch();
    refresh();
}
