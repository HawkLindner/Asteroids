    // for initializing and shutdown functions
#include <SDL2/SDL.h>

// for rendering images and graphics on screen
#include <SDL2/SDL_image.h>

// for using SDL_Delay() functions
#include <SDL2/SDL_timer.h>

#include <SDL2/SDL_mixer.h>

#include <SDL2/SDL_ttf.h>

#include "game.h"
#include "bullet.h"
#include "asteroid.h"

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib> // For random number generation
#include <ctime>
using namespace std;
    
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;

const double PI = 3.14159265;
//function to convert degrees to radians
inline double toRadians(double degrees) { return degrees * PI / 180.0; }
void startScreen(){
        //initializes sdl
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return;
    }
        //if the text cannot init send error
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

    // Load the font from assets folder
    TTF_Font *font = TTF_OpenFont("assets/fonts/text.ttf", 48); // Path to your font file
    if (!font) {
        cout << "Failed to load font! TTF_Error: " << TTF_GetError() << endl;
        return;
    }


    
    //sets font color
    SDL_Color textColor = {255, 255, 255, 255};

    //for start screen
    bool isStartScreen = true;
    //for keyboard
    SDL_Event event;
    //starts a time for the rand()
    srand(static_cast<unsigned>(time(0)));
    vector<asteroid> startScreenAsteroids;
    srand(static_cast<unsigned>(time(0)));
    for (int i = 0; i < 10; ++i) {
        double startX = rand() % SCREEN_WIDTH;
        double startY = rand() % SCREEN_HEIGHT;
        double velocityX = (rand() % 3 + 1) * (rand() % 2 == 0 ? 1 : -1);
        double velocityY = (rand() % 3 + 1) * (rand() % 2 == 0 ? 1 : -1);
        int size = 3;
        startScreenAsteroids.emplace_back(startX, startY, velocityX, velocityY, size);
    }
    
    while (isStartScreen) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isStartScreen = false;
            return;
        }

        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
            isStartScreen = false; // Exit start screen when Enter is pressed
            return;
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

    // Render centered text
    SDL_Color white = {255, 255, 255, 255}; // White color
    SDL_Texture* textTexture = renderText(renderer, font, "ASTEROIDS", white);
    if (textTexture) {
        int textWidth, textHeight;
        SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);

        // Center the text on the screen
        SDL_Rect textRect = {
            SCREEN_WIDTH / 2 - textWidth / 2,
            SCREEN_HEIGHT / 2 - textHeight / 2,
            textWidth,
            textHeight
        };

        SDL_RenderCopy(renderer, textTexture, NULL, &textRect); // Render the text
        SDL_DestroyTexture(textTexture); // Free the texture after rendering
    }

    // Render prompt text
    SDL_Texture* promptTexture = renderText(renderer, font, "Press Enter to Start", white);
    if (promptTexture) {
        int promptWidth, promptHeight;
        SDL_QueryTexture(promptTexture, NULL, NULL, &promptWidth, &promptHeight);

        SDL_Rect promptRect = {
            SCREEN_WIDTH / 2 - promptWidth / 2,
            SCREEN_HEIGHT / 2 + 50,
            promptWidth,
            promptHeight
        };

        SDL_RenderCopy(renderer, promptTexture, NULL, &promptRect); // Render the prompt
        SDL_DestroyTexture(promptTexture); // Free the texture after rendering
    }

    // Present the frame
    SDL_RenderPresent(renderer);

    // Delay to cap frame rate
    SDL_Delay(16); // Cap at ~60 FPS
}


      // Cleanup
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return;
}