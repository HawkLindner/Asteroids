// for initializing and shutdown functions
#include <SDL2/SDL.h>
// for rendering images and graphics on screen
#include <SDL2/SDL_image.h>
// for using SDL_Delay() functions
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>


//Using SDL, SDL_image, SDL_ttf, standard IO, math, and strings

#include <stdio.h>
#include "game.h"
#include "bullet.h"
#include "asteroid.h"
#include "constants.h"

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib> // For random number generation
#include <ctime>
using namespace std;



/*
    Manages the game loop (update, render, handle input).
    Acts as the central hub for all game objects (ship, asteroids, bullets).
*/
bool endGame(){
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return true;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        cout << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << endl;
        SDL_Quit();
        return true;
    }
    SDL_Window* end = SDL_CreateWindow("Asteroids Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* rend = SDL_CreateRenderer(end,-1,SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("assets/fonts/orig.ttf", 36);
    SDL_Color white = {255,255,255,255};
    if (!end) {
        cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        SDL_Quit();
        return true;
    }
    if (!rend) {
        cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        SDL_DestroyWindow(end);
        SDL_Quit();
        return true;
    }
    if (!font) {
        cout << "Failed to load font! TTF_Error: " << TTF_GetError() << endl;
        return true;
    }

    // Initialize asteroid vector
    vector<asteroid> startScreenAsteroids;
    srand(static_cast<unsigned>(time(0)));
    for (int i = 0; i < 10; ++i) {
        double startX = rand() % SCREEN_WIDTH;
        double startY = rand() % SCREEN_HEIGHT;
        double velocityX = (rand() % 3 + 1) * (rand() % 2 == 0 ? 1 : -1);
        double velocityY = (rand() % 3 + 1) * (rand() % 2 == 0 ? 1 : -1);
        int size = 3; // Large asteroid
        startScreenAsteroids.emplace_back(startX, startY, velocityX, velocityY, size);
    }

    // Start screen loop
    bool endgame = true;
    while (endgame) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE ) {
                return true;
            }

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                return false;
            }
        }

        // Update asteroid positions
        for (auto& asteroid : startScreenAsteroids) {
            asteroid.update(); // Update asteroid positions
        }

        // Clear the screen
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255); // Black background
        SDL_RenderClear(rend);

        // Render moving asteroids in the background
        for (auto& asteroid : startScreenAsteroids) {
            asteroid.render(rend); // Render asteroids
        }

        // Render centered game title
        SDL_Texture* titleTexture = renderText(rend, font, "GAME OVER", white);
        if (titleTexture) {
            int textWidth, textHeight;
            SDL_QueryTexture(titleTexture, NULL, NULL, &textWidth, &textHeight);

            SDL_Rect titleRect = {
                SCREEN_WIDTH / 2 - textWidth / 2,
                SCREEN_HEIGHT / 2 - textHeight / 2 - 50, // Slightly above center
                textWidth,
                textHeight
            };

            SDL_RenderCopy(rend, titleTexture, NULL, &titleRect);
            SDL_DestroyTexture(titleTexture); // Free the texture after rendering
        }

        // Render "Press Enter to Start" prompt
        SDL_Texture* promptTexture = renderText(rend, font, "Press Enter to Restart", white);
        if (promptTexture) {
            int promptWidth, promptHeight;
            SDL_QueryTexture(promptTexture, NULL, NULL, &promptWidth, &promptHeight);

            SDL_Rect promptRect = {
                SCREEN_WIDTH / 2 - promptWidth / 2,
                SCREEN_HEIGHT / 2 + 50, // Slightly below center
                promptWidth,
                promptHeight
            };

            SDL_RenderCopy(rend, promptTexture, NULL, &promptRect);
            SDL_DestroyTexture(promptTexture); // Free the texture after rendering
        }

        // Present the frame
        SDL_RenderPresent(rend);

        // Delay to cap frame rate
        SDL_Delay(16); // Cap at ~60 FPS
    }

    // Cleanup resources
    TTF_CloseFont(font);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(end);
    TTF_Quit();
    SDL_Quit();
}
