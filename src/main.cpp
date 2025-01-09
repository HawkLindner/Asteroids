// for initializing and shutdown functions
#include <SDL2/SDL.h>
// for rendering images and graphics on screen
#include <SDL2/SDL_image.h>
// for using SDL_Delay() functions
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "game.h"
#include "mainScreen.cpp"
#include "endScreen.cpp"
#include "shared_context.h"

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;


// Define the global variables here
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;
/*
    This is my program of making asteroids using C++

    COMPILE: g++ -Wall -std=c++17 $(sdl2-config --cflags --libs) -lSDL2_image make.cpp -o ast
*/
int main(int argc, char*argv[])
{
        // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create the window and renderer
    window = SDL_CreateWindow("Asteroids", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 800, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Load a font
    font = TTF_OpenFont("assets/fonts/orig.ttf", 36);
    if (!font) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    cout<<"Welcome"<<endl;
    bool end = true;
    startScreen();
    createGame();
    end = endGame();
    while(end!= true){
        createGame();
        end = endGame();
    }
    // Cleanup
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
