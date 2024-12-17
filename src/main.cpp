

// for initializing and shutdown functions
#include <SDL2/SDL.h>

// for rendering images and graphics on screen
#include <SDL2/SDL_image.h>

// for using SDL_Delay() functions
#include <SDL2/SDL_timer.h>

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

/*
    This is my program of making asteroids using C++

    COMPILE: g++ -Wall -std=c++17 $(sdl2-config --cflags --libs) -lSDL2_image make.cpp -o ast
*/
int main(int argc, char *argv[])
{
    cout<<"Welcome"<<endl;
    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window* game = SDL_CreateWindow("Asteroids",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        1200, 900, 0);

    if (!game) {
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Main loop flag
    bool running = true;
    SDL_Event event;

    // Main game loop
    while (running) {
        // Event handling
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false; // Exit on window close
            }

            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = false; // Exit on ESC key
                        break;
                    case SDLK_w:
                        cout << "W key pressed" << endl;
                        break;
                    default:
                        break;
                }
            }
        }

        SDL_Delay(16); // Roughly 60 FPS
    }

    // Cleanup and exit
    SDL_DestroyWindow(game);
    SDL_Quit();
    return 0;
}
