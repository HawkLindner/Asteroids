

// for initializing and shutdown functions
#include <SDL2/SDL.h>

// for rendering images and graphics on screen
#include <SDL2/SDL_image.h>

// for using SDL_Delay() functions
#include <SDL2/SDL_timer.h>

#include "game.h"

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

/*
    This is my program of making asteroids using C++

    COMPILE: g++ -Wall -std=c++17 $(sdl2-config --cflags --libs) -lSDL2_image make.cpp -o ast
*/
int main()
{
    cout<<"Welcome"<<endl;
    createGame();
    return 0;
}
