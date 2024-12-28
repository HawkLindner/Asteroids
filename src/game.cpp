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

/*
    Manages the game loop (update, render, handle input).
    Acts as the central hub for all game objects (ship, asteroids, bullets).
*/

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;

const double PI = 3.14159265;
//function to convert degrees to radians
inline double toRadians(double degrees) { return degrees * PI / 180.0; }


/*
    This function is used to render the text that will display the players lives
*/
SDL_Texture* renderText(SDL_Renderer* renderer, TTF_Font* font, const string& text, int x, int y, SDL_Color color) {
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!textSurface) {
        cout << "Failed to create text surface! TTF_Error: " << TTF_GetError() << endl;
        return NULL;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    if (!textTexture) {
        cout << "Failed to create text texture! SDL_Error: " << SDL_GetError() << endl;
        return NULL;
    }

    SDL_Rect renderQuad = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
    return textTexture;
}


void drawStartingShip(double x, double y, double angle,SDL_Renderer* renderer){
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
}

void checkDamage(vector<asteroid> &asteroids, vector<bullet> &bullets,int &playerLives, double x, double y,double velocityX, double velocityY, bool &gameOver
                ,SDL_Renderer* renderer, double angle){
    (void)velocityX;
    (void)velocityY;
    (void)gameOver;
    std::vector<asteroid> newAsteroids; // Temporary vector to store new asteroids
    for (auto itAst = asteroids.begin(); itAst != asteroids.end();) {
        bool asteroidDestroyed = false;

        // 1. Check Bullet-Asteroid Collision
        for (auto itBullet = bullets.begin(); itBullet != bullets.end();) {
            double dx = itBullet->getX() - itAst->getX();
            double dy = itBullet->getY() - itAst->getY();
            double distance = sqrt(dx * dx + dy * dy);

            if (distance < itAst->getRad()) {
                // Bullet hits asteroid
                std::vector<asteroid> fragments = itAst->split();
                newAsteroids.insert(newAsteroids.end(), fragments.begin(), fragments.end());

                itBullet = bullets.erase(itBullet); // Safe erase: remove bullet
                asteroidDestroyed = true; // Mark asteroid for removal
                break; // Exit bullet loop
            } else {
                ++itBullet; // Increment bullet iterator
            }
        }

        // 2. Check Ship-Asteroid Collision
        double shipDx = x - itAst->getX();
        double shipDy = y - itAst->getY();
        double shipDistance = sqrt(shipDx * shipDx + shipDy * shipDy);

        if (shipDistance < itAst->getRad()) {
            // Ship hits asteroid
            playerLives--;


            if (playerLives == 0) {
                gameOver = true; // Trigger game over
                break;
            }


            // Reset ship position and velocity
            x = SCREEN_WIDTH / 2;
            y = SCREEN_HEIGHT / 2;
            velocityX = 0;
            velocityY = 0;
            drawStartingShip(x,y,angle,renderer);

            asteroidDestroyed = true; // Optionally destroy asteroid
        }

        // Remove asteroid if destroyed or inactive
        if (asteroidDestroyed || !itAst->isActive()) {
            itAst = asteroids.erase(itAst); // Safe erase: remove asteroid
        } else {
            ++itAst; // Increment asteroid iterator
        }
    }

    // Add newly created asteroids to the main vector
    asteroids.insert(asteroids.end(), newAsteroids.begin(), newAsteroids.end());
}
/*
    This will be the function for the game
*/
void createGame(){

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

    //Initialize shooting sound
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

    // Load the font from assets folder
    TTF_Font *font = TTF_OpenFont("assets/stocky/stocky.ttf", 100); // Path to your font file
    if (!font) {
        cout << "Failed to load font! TTF_Error: " << TTF_GetError() << endl;
        TTF_Quit();
        return;
    }
    
    //sets font color
    SDL_Color textColor = {255, 255, 255, 255};

    // Main loop flag
    bool running = true;
    //for start screen
    bool isStartScreen = true;
    //if the ship is not moving it will slow down 
    bool isMoving = false;
    //if the space is pressed, we are shooting a bullet
    bool spacePressed = false;
    //how many player lives until they lose
    int playerLives = 3;
    //if player lives gets to 0, game over
    bool gameOver = false;
    //for keyboard
    SDL_Event event;
    //starts the ship in the center
    double x = SCREEN_WIDTH / 2;
    double y = SCREEN_HEIGHT / 2;
    //speed limit for the ship
    const double speedLimit = 3.0;
    //percent to slow down the ship
    const double friction = 0.98;
    //starts the ship facing up
    double angle = 270.0; 
    //speed in the x and y directions
    double velocityX = 0, velocityY = 0;
    //thrust speed
    const double speed = 0.1; 
    //how fast the ship turns
    const double rotationSpeed = 3.0;

    //vectors for bullets and asteroids
    vector<bullet> bullets;
    vector<asteroid> asteroids;
    SDL_Texture *textTexture;

    //starts a time for the rand()
    srand(static_cast<unsigned>(time(0)));

    // Spawn initial asteroids, needs to be worked on for levels
    for (int i = 0; i < 15; ++i) {
        double startX = rand() % SCREEN_WIDTH;
        double startY = rand() % SCREEN_HEIGHT;
        double velocityX = (rand() % 5 + 1) * (rand() % 2 == 0 ? 1 : -1); // Random speed and direction
        double velocityY = (rand() % 5 + 1) * (rand() % 2 == 0 ? 1 : -1);
        int size = 3; // Large asteroid
        asteroids.emplace_back(startX, startY, velocityX, velocityY, size);
        
    }
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
    while (isStartScreen && running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                SDL_DestroyTexture(textTexture);
                return;
            }

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                isStartScreen = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);
        // Render title and prompt
        
        textTexture = renderText(renderer, font, "ASTEROIDS", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 100, textColor);
        renderText(renderer, font, "Press Enter to Start", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2, textColor);

        
        // Render asteroids
        for (auto& asteroid : startScreenAsteroids) {
            asteroid.update();
            asteroid.render(renderer);
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(16); // Cap ~60 FPS
}


    // Game loop, while running is true, we loop
    while (running) {
        // Handle events such as keyboard
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false; // Exit when the window is closed
            }
        }
        // Key Handling, gets the state of the keyboard
        const Uint8* state = SDL_GetKeyboardState(NULL);
        //if escape close the loop and end program
        if (state[SDL_SCANCODE_ESCAPE]){
            running = false;
        }
        //if we let go of a w,a,s,d key the ship is no longer influenced moving
        if (state[SDL_KEYUP]){
            isMoving = false;
        }
        //moves the ship in the direction that the ships nose is in
        if (state[SDL_SCANCODE_W]) {
            // Move forward in the facing direction
            velocityX += cos(toRadians(angle)) * speed;
            velocityY += sin(toRadians(angle)) * speed;
            isMoving = true;

        }
        //moves the ship in the opposite direction of the nose
        if (state[SDL_SCANCODE_S]) {
            // Move backward (opposite of the facing direction)
            velocityX -= cos(toRadians(angle)) * speed;
            velocityY -= sin(toRadians(angle)) * speed;
            isMoving = true;

        }
        //turns the nose of the ship left
        if (state[SDL_SCANCODE_A]) {
            angle -= rotationSpeed;
            if (angle < 0) angle += 360.0;
        }
        //turns the nose of the ship right
        if (state[SDL_SCANCODE_D]) {
            angle += rotationSpeed;
            if (angle >= 360) angle -= 360.0;
        }
        //Space pressed turns to true, bullets get fired
        if (state[SDL_SCANCODE_SPACE] && !spacePressed){
            bullets.emplace_back(x, y, angle);
            Mix_PlayChannel(-1, shootSound, 0); // Play shooting sound
            spacePressed = true;
        } 
        //if space isnt pressed then spacePressed is false
        if (!state[SDL_SCANCODE_SPACE]){
            spacePressed = false;
        }
        //if we are not pressing a key to move, then we will slowly glide to a stop
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
        for (auto& asteroid : asteroids) asteroid.update();

        // Remove inactive bullets
        bullets.erase(remove_if(bullets.begin(), bullets.end(),
                                        [](const bullet& b) { return !b.isActive(); }),
                                        bullets.end());


        // Rendering
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear screen to black
        SDL_RenderClear(renderer);

        // Draw Ship (Triangle)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Ship color (white)

          // Render the remaining lives
        string livesText = "Lives: " + to_string(playerLives);
        renderText(renderer, font, livesText, 10, 10, textColor);

        drawStartingShip(x,y,angle,renderer);

        checkDamage(asteroids,bullets,playerLives,x,y,velocityX,velocityY,gameOver,renderer,angle);

        for (auto& bullet : bullets) bullet.render(renderer);
        for (auto& asteroid : asteroids) asteroid.render(renderer);


        if (gameOver) {
            // Clear the screen
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            // Keep asteroids floating
            for (auto& asteroid : asteroids) {
                asteroid.update();  // Update asteroid positions
                asteroid.render(renderer);  // Render asteroids
            }

            // Display "Game Over" message
            string gameOverText = "GAME OVER";
            string restartText = "Press Enter to Restart";
            renderText(renderer, font, gameOverText, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, textColor);
            renderText(renderer, font, restartText, SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 20, textColor);

            SDL_RenderPresent(renderer); // Display everything

            // Wait for Enter key to restart the game
            SDL_Event event;
            while (true) {
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        running = false; // Exit the main game loop
                        return;
                    }

                    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                        // Reset the game state
                        playerLives = 3;
                        gameOver = false;

                        // Reset ship position and velocity
                        x = SCREEN_WIDTH / 2;
                        y = SCREEN_HEIGHT / 2;
                        velocityX = 0;
                        velocityY = 0;

                        // Clear all bullets (preserve asteroid motion)
                        bullets.clear();

                        return; // Exit the Game Over state and restart the game
                    }
                }

                SDL_Delay(100); // Add a small delay to reduce CPU usage
            }
        }

        
        // Update Screen
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // Cap ~60 FPS
    }

  // Cleanup
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
