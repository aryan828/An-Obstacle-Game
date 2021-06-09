#include "../include/FileManager.h"


void FileManager::writeScore(Player player) {
    std::ofstream file;
    file.open("scores", std::fstream::app);
    file << "\n" << player.player_name << " " << player.score;
    file.close();
}

void FileManager::readScores(WINDOW *screen) {
    try {
        std::ifstream file("scores");
        Player player;
        int i=0;
        std::string emp;
        std::getline(file, emp);
        while(!file.eof() && i<10) {
            file >> player.player_name >> player.score;
            i++;
            mvwprintw(screen, 4+i, 35, "%s", player.player_name);
            mvwprintw(screen, 4+i, 46, "%d", player.score);
        }
        if(i==0) throw "No scores available!";
    } catch (const char *error) {
        mvwprintw(screen, 6, 31, "%s", error);
    }
}
