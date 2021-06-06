cmake_minimum_required(VERSION 3.16)
project(TheOPGame)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}" -pthread )

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

find_library(MENU_LIBRARY menu)
find_library(FORM_LIBRARY form)

add_executable(TheOPGame ${SOURCES} src/player.cpp include/player.h src/FileManager.cpp include/FileManager.h )
target_link_libraries(TheOPGame ${CURSES_LIBRARIES} ${MENU_LIBRARY} ${FORM_LIBRARY})

find_package(Threads REQUIRED)
target_link_libraries(TheOPGame Threads::Threads)

#add_executable(TheOPGame main.cpp game.cpp game.h screen.cpp screen.h player_controller.cpp player_controller.h obstacle.cpp obstacle.h collectible.cpp collectible.h board_generator.cpp board_generator.h)