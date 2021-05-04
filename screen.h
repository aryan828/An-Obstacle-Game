#ifndef SCREEN_H
#define SCREEN_H

#include <menu.h>
#include <vector>
#include <cstdlib>

class Screen {
    virtual void initScreen() = 0;

protected:
    WINDOW *screen;
};

class GameScreen: Screen {
    int xPos = 0;
    int yPos = 0;
public:
    GameScreen() {
        xPos = 1;
        yPos = 1;
        screen = newwin(33, 87, yPos, xPos);
        box(screen, 0, 0);
    }
    void initScreen() override {
        refresh();
        wprintw(screen, "Game Screen Works!");
        wrefresh(screen);
        getch();
    }
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
    std::vector<char*> messages {"NEW GAME", "SCORES", "EXIT"};

public:
    WelcomeScreen() {
        xPos = 1;
        yPos = 1;
        screen = newwin(33, 87, yPos, xPos);
        box(screen, 0, 0);
    }
    void initScreen() override {
        refresh();
        wrefresh(screen);
        printMenu();
    }

    void printMenu() {
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
                        GameScreen gameScreen;
                        gameScreen.initScreen();
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

};

#endif //SCREEN_H
