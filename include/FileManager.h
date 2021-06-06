//
// Created by aryan on 06/06/21.
//

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <fstream>
#include <string>
#include <iostream>
#include "player.h"
#include "curses.h"

class FileManager{
public:
    static void writeScore(Player player);

    static void readScores(WINDOW *screen);
};

#endif //FILEMANAGER_H
