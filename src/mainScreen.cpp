    // for initializing and shutdown functions
#include <SDL2/SDL.h>

// for rendering images and graphics on screen
#include <SDL2/SDL_image.h>

// for using SDL_Delay() functions
#include <SDL2/SDL_timer.h>

#include <SDL2/SDL_mixer.h>

#include <SDL2/SDL_ttf.h>

#include "game.h"
#include "constants.h"
#include "bullet.h"
#include "asteroid.h"


#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib> // For random number generation
#include <ctime>
using namespace std;


void startScreen() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        cout << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << endl;
        SDL_Quit();
        return;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("Asteroids Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        SDL_Quit();
        return;
    }

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    // Load fonts
    TTF_Font* Lfont = TTF_OpenFont("assets/fonts/orig.ttf", 100); // Large font
    TTF_Font* Sfont = TTF_OpenFont("assets/fonts/orig.ttf", 24); // Small font
    if (!Lfont || !Sfont) {
        cout << "Failed to load font! TTF_Error: " << TTF_GetError() << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return;
    }


    // Set text color
    SDL_Color white = {255, 255, 255, 255}; // White color

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
    bool isStartScreen = true;
    while (isStartScreen) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isStartScreen = false; // Quit the start screen
                break;
            }

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                isStartScreen = false; // Exit start screen when Enter is pressed
                break;
            }
        }

        // Update asteroid positions
        for (auto& asteroid : startScreenAsteroids) {
            asteroid.update(); // Update asteroid positions
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        // Render moving asteroids in the background
        for (auto& asteroid : startScreenAsteroids) {
            asteroid.render(renderer); // Render asteroids
        }

        // Render centered game title
        SDL_Texture* titleTexture = renderText(renderer, Lfont, "ASTEROIDS", white);
        if (titleTexture) {
            int textWidth, textHeight;
            SDL_QueryTexture(titleTexture, NULL, NULL, &textWidth, &textHeight);

            SDL_Rect titleRect = {
                SCREEN_WIDTH / 2 - textWidth / 2,
                SCREEN_HEIGHT / 2 - textHeight / 2 - 50, // Slightly above center
                textWidth,
                textHeight
            };

            SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
            SDL_DestroyTexture(titleTexture); // Free the texture after rendering
        }

        // Render "Press Enter to Start" prompt
        SDL_Texture* promptTexture = renderText(renderer, Sfont, "Press Enter to Start", white);
        if (promptTexture) {
            int promptWidth, promptHeight;
            SDL_QueryTexture(promptTexture, NULL, NULL, &promptWidth, &promptHeight);

            SDL_Rect promptRect = {
                SCREEN_WIDTH / 2 - promptWidth / 2,
                SCREEN_HEIGHT / 2 + 50, // Slightly below center
                promptWidth,
                promptHeight
            };

            SDL_RenderCopy(renderer, promptTexture, NULL, &promptRect);
            SDL_DestroyTexture(promptTexture); // Free the texture after rendering
        }

        // Present the frame
        SDL_RenderPresent(renderer);

        // Delay to cap frame rate
        SDL_Delay(16); // Cap at ~60 FPS
    }

    // Cleanup resources
    TTF_CloseFont(Lfont);
    TTF_CloseFont(Sfont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}
