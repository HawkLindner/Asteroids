// for initializing and shutdown functions
#include <SDL2/SDL.h>

// for rendering images and graphics on screen
#include <SDL2/SDL_image.h>

// for using SDL_Delay() functions
#include <SDL2/SDL_timer.h>

#include <SDL2/SDL_mixer.h>


#include "game.h"
#include "bullet.h"

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

/*
    Manages the game loop (update, render, handle input).
    Acts as the central hub for all game objects (ship, asteroids, bullets).
*/

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;

const double PI = 3.14159265;
inline double toRadians(double degrees) { return degrees * PI / 180.0; }

void createGame(){
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
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

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    // Load shooting sound
    Mix_Chunk* shootSound = Mix_LoadWAV("assets/shoot.wav");
    if (!shootSound) {
        cout << "Failed to load shooting sound! SDL_mixer Error: " << Mix_GetError() << endl;
        Mix_CloseAudio();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        
        SDL_Quit();
        return;
    }

    // Main loop flag
    bool running = true;
    bool isMoving = false;
    bool spacePressed = false;
    SDL_Event event;

     // Ship State
    double x = SCREEN_WIDTH / 2;
    double y = SCREEN_HEIGHT / 2;
    const double speedLimit = 4.0;
    const double friction = 0.98;
    double angle = 270.0; // Start facing "up"
    double velocityX = 0, velocityY = 0;
    const double speed = 0.1; // Thrust speed
    const double rotationSpeed = 3.0;


    vector<bullet> bullets;
    // Game loop
while (running) {
    // Handle events
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false; // Exit when the window is closed
        }
    }

    // Key Handling
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_ESCAPE]){
        running = false;
    }
    if (state[SDL_KEYUP]){
        isMoving = false;
    }
    if (state[SDL_SCANCODE_W]) {
        // Move forward in the facing direction
        velocityX += cos(toRadians(angle)) * speed;
        velocityY += sin(toRadians(angle)) * speed;
        isMoving = true;

    }
    if (state[SDL_SCANCODE_S]) {
        // Move backward (opposite of the facing direction)
        velocityX -= cos(toRadians(angle)) * speed;
        velocityY -= sin(toRadians(angle)) * speed;
        isMoving = true;

    }
    if (state[SDL_SCANCODE_A]) {
        // Turn left
        angle -= rotationSpeed;
        if (angle < 0) angle += 360.0;
    }
    if (state[SDL_SCANCODE_D]) {
        // Turn right
        angle += rotationSpeed;
        if (angle >= 360) angle -= 360.0;
    }
    if (state[SDL_SCANCODE_SPACE] && !spacePressed){
        bullets.emplace_back(x, y, angle);
        spacePressed = true;
    } 
    if (!state[SDL_SCANCODE_SPACE]){
        spacePressed = false;
    }

    if (!isMoving) {
    velocityX *= friction;
    velocityY *= friction;

    // Stop very small velocities to avoid drifting
    if (fabs(velocityX) < 0.01) velocityX = 0;
    if (fabs(velocityY) < 0.01) velocityY = 0;
}

    double speedMagnitude = sqrt(velocityX * velocityX + velocityY * velocityY);

    // Apply speed limit
    if (speedMagnitude > speedLimit) {
        velocityX = velocityX * (speedLimit / speedMagnitude);
        velocityY = velocityY * (speedLimit / speedMagnitude);
    }
    // Update position
    x += velocityX;
    y += velocityY;


    // Screen Wrap-around
    if (x > SCREEN_WIDTH) x = 0;
    if (x < 0) x = SCREEN_WIDTH;
    if (y > SCREEN_HEIGHT) y = 0;
    if (y < 0) y = SCREEN_HEIGHT;

    for (auto& bullet : bullets) bullet.update();


        // Remove inactive bullets
        bullets.erase(remove_if(bullets.begin(), bullets.end(),
                                        [](const bullet& b) { return !b.isActive(); }),
                                        bullets.end());


    // Rendering
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear screen to black
    SDL_RenderClear(renderer);

    // Draw Ship (Triangle)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Ship color (white)

    // Calculate triangle points
    SDL_Point points[4]; // 3 vertices + closing line
    points[0] = { static_cast<int>(x + cos(toRadians(angle)) * 20), 
                  static_cast<int>(y + sin(toRadians(angle)) * 20) }; // Tip of the triangle
    points[1] = { static_cast<int>(x + cos(toRadians(angle + 140)) * 15), 
                  static_cast<int>(y + sin(toRadians(angle + 140)) * 15) }; // Left vertex
    points[2] = { static_cast<int>(x + cos(toRadians(angle - 140)) * 15), 
                  static_cast<int>(y + sin(toRadians(angle - 140)) * 15) }; // Right vertex
    points[3] = points[0]; // Close the triangle

    SDL_RenderDrawLines(renderer, points, 4);

    for (auto& bullet : bullets) bullet.render(renderer);

    // Update Screen
    SDL_RenderPresent(renderer);

    SDL_Delay(16); // Cap ~60 FPS
}


    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}