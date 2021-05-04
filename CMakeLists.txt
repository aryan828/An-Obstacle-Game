cmake_minimum_required(VERSION 3.19)
project(TheOPGame)

set(CMAKE_CXX_STANDARD 20)

add_executable(TheOPGame main.cpp game.cpp game.h screen.cpp screen.h player_controller.cpp player_controller.h obstacle.cpp obstacle.h collectible.cpp collectible.h)