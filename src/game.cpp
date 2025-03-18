#include "game.h"
#include "constants.h"
#include "bullet.h"
#include "asteroid.h"
#include "shared_context.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib> // For random number generation
#include <ctime>

using std::cout;
using std::vector;
using std::string;

/*
    Manages the game loop (update, render, handle input).
    Acts as the central hub for all game objects (ship, asteroids, bullets).
*/
// Key Handling, gets the state of the keyboard
const Uint8* state = SDL_GetKeyboardState(NULL);

/*
    This function is used to render the text that will display the players lives
*/
SDL_Texture* renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color) {
    // Render the text to a surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!textSurface) {
        std::cout << "Failed to create text surface! TTF_Error: " << TTF_GetError() << std::endl;
        return nullptr;
    }

    // Convert surface to texture
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface); // Free the surface as it's no longer needed

    if (!textTexture) {
        std::cout << "Failed to create text texture! SDL_Error: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    return textTexture;
}

void renderTextOnScreen(SDL_Renderer* renderer, SDL_Texture* textTexture, int x, int y) {
    // Query the width and height of the texture
    int width, height;
    SDL_QueryTexture(textTexture, nullptr, nullptr, &width, &height);

    // Set the position and size of the text
    SDL_Rect renderQuad = {x, y, width, height};

    // Render the text on the screen
    SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
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
/*
    This is a helper function that will allow my check damage to render a circle to show the explosion
*/
void SDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // Horizontal offset
            int dy = radius - h; // Vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

void checkDamage(vector<asteroid> &asteroids, vector<bullet> &bullets, int &playerLives, double &x, double &y, double &velocityX,double &velocityY, bool &running, SDL_Renderer* renderer, double angle, int &playerScore,Mix_Chunk* largeShoot,Mix_Chunk* mediumShoot, Mix_Chunk* smallShoot
) {
    std::vector<asteroid> newAsteroids; // Temporary vector to store new asteroids

    for (auto itAst = asteroids.begin(); itAst != asteroids.end();) {
        bool asteroidDestroyed = false;

        // 1. Check Bullet-Asteroid Collision
        for (auto itBullet = bullets.begin(); itBullet != bullets.end();) {
            double dx = itBullet->getX() - itAst->getX();
            double dy = itBullet->getY() - itAst->getY();
            double distance = sqrt(dx * dx + dy * dy);

            if (distance < itAst->getRad()) {
                int asteroidSize = itAst->getSize();
                if (asteroidSize == 3) {
                    playerScore += 15;
                    Mix_PlayChannel(-1, largeShoot, 0); // Play shooting sound
                } else if (asteroidSize == 2) {
                    playerScore += 35;
                    Mix_PlayChannel(-1, mediumShoot, 0); // Play shooting sound
                } else if (asteroidSize == 1) {
                    playerScore += 50;
                    Mix_PlayChannel(-1, smallShoot, 0); // Play shooting sound
                }

                // Bullet hits asteroid
                std::vector<asteroid> fragments = itAst->split();
                newAsteroids.insert(newAsteroids.end(), fragments.begin(), fragments.end());

                itBullet = bullets.erase(itBullet); // Remove the bullet
                asteroidDestroyed = true; // Mark asteroid for removal
                playerScore += 100;
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

            // Render explosion effect
            const int explosionFrames = 30; // Duration of explosion effect
            for (int i = 0; i < explosionFrames; ++i) {
                SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255); // Orange color for explosion
                SDL_RenderDrawPoint(renderer, static_cast<int>(x), static_cast<int>(y)); // Explosion center
                SDL_RenderDrawCircle(renderer, static_cast<int>(x), static_cast<int>(y), i * 3); // Expanding circle
                SDL_RenderPresent(renderer); // Show the explosion
                SDL_Delay(16); // Frame delay (~60 FPS)
            }

            // Reset ship position and velocity after explosion
            x = SCREEN_WIDTH / 2;
            y = SCREEN_HEIGHT / 2;
            velocityX = 0;
            velocityY = 0;

            if (playerLives == 0) {
                running = false;
                break;
            }

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

    // Main loop flag
    bool running = true;
    //if the ship is not moving it will slow down 
    bool isMoving = false;
    //if the space is pressed, we are shooting a bullet
    bool spacePressed = false;
    //starts the ship in the center
    double x = SCREEN_WIDTH / 2;
    double y = SCREEN_HEIGHT / 2;
    double speedMagnitude;
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

    double scoreMultiplier = 2.5;
    //how many player lives until they lose
    int playerLives = 3;

    int scoreForNextLevel = 1000;
    //sets player score
    int playerScore = 0;
    int level = 1;
    int maxLevel = 20;
    //creates a Mix_Chunk for the sound of the shooting
    Mix_Chunk* shootSound;
    //for keyboard
    SDL_Event event;    
    //vectors for bullets and asteroids
    vector<bullet> bullets;
    vector<asteroid> asteroids;

    SDL_Color white = {255, 255, 255, 255}; // White color


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
    window = SDL_CreateWindow("Asteroids Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        SDL_Quit();
        return;
    }

    // Create a renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
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
    shootSound = Mix_LoadWAV("assets/shoot.wav");
    if (!shootSound) {
        cout << "Failed to load shooting sound! SDL_mixer Error: " << Mix_GetError() << endl;
        Mix_CloseAudio();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }
    Mix_Chunk* largeShoot = Mix_LoadWAV("assets/largeShoot.mp3");
    if(!largeShoot){
        cout << "Failed to load shooting sound! SDL_mixer Error: " << Mix_GetError() << endl;
        Mix_CloseAudio();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }
    Mix_Chunk* mediumShoot = Mix_LoadWAV("assets/mediumShoot.mp3");
    if(!mediumShoot){
        cout << "Failed to load shooting sound! SDL_mixer Error: " << Mix_GetError() << endl;
        Mix_CloseAudio();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }
    Mix_Chunk* smallShoot = Mix_LoadWAV("assets/smallShoot.mp3");
    if(!smallShoot){
        cout << "Failed to load shooting sound! SDL_mixer Error: " << Mix_GetError() << endl;
        Mix_CloseAudio();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    // Load the font from assets folder
    font = TTF_OpenFont("assets/fonts/orig.ttf", 36); // Path to your font file
    if (!font) {
        cout << "Failed to load font! TTF_Error: " << TTF_GetError() << endl;
        return;
    }

    //draws the starting ship once before the loops start
    drawStartingShip(x,y,angle,renderer);

    for (int i = 0; i < 5; ++i) {
            double startX = rand() % SCREEN_WIDTH;
            double startY = rand() % SCREEN_HEIGHT;
            double velocityX = (rand() % (3 + level)) * (rand() % 2 == 0 ? 1 : -1); // Faster asteroids
            double velocityY = (rand() % (3 + level)) * (rand() % 2 == 0 ? 1 : -1);
            int size = 3; // Large asteroid
            asteroids.emplace_back(startX, startY, velocityX, velocityY, size);
        }
    // Game loop, while running is true, we loop
    while (running) {
        // Handle events such as keyboard
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false; // Exit when the window is closed
            }
        }
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

        // Screen Wrap-around
        if (x > SCREEN_WIDTH) x = 0;
        if (x < 0) x = SCREEN_WIDTH;
        if (y > SCREEN_HEIGHT) y = 0;
        if (y < 0) y = SCREEN_HEIGHT;

        //speed of the ship
        speedMagnitude = sqrt(velocityX * velocityX + velocityY * velocityY);

        // Apply speed limit
        if (speedMagnitude > speedLimit) {
            velocityX = velocityX * (speedLimit / speedMagnitude);
            velocityY = velocityY * (speedLimit / speedMagnitude);
        }
        // Update position
        x += velocityX;
        y += velocityY;

        //this updates the bullet and the asteroid vectors
        for (auto& bullet : bullets) bullet.update();
        for (auto& asteroid : asteroids) asteroid.update();

        // Remove inactive bullets
        bullets.erase(remove_if(bullets.begin(), bullets.end(),
                                        [](const bullet& b) { return !b.isActive(); }),
                                        bullets.end());

        //This is how i display the player Lives
        string livesText = to_string(playerLives);

        // Rendering
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear screen to black
        SDL_RenderClear(renderer);

        // Draw Ship (Triangle)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Ship color (white)

        // Render the remaining lives
        SDL_Texture* titleTexture = renderText(renderer, font, "Lives  " + livesText, white);
        if (titleTexture) {
            int textWidth, textHeight;
            SDL_QueryTexture(titleTexture, NULL, NULL, &textWidth, &textHeight);

            SDL_Rect titleRect = {
                10,10,textWidth, textHeight
            };

            SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
            SDL_DestroyTexture(titleTexture); // Free the texture after rendering
        }
        string scoreText = to_string(playerScore);
        SDL_Texture* score = renderText(renderer, font, "Score " + scoreText,white);
        if(score){
            int textWidth, textHeight;
            SDL_QueryTexture(score,NULL,NULL,&textWidth,&textHeight);
            SDL_Rect titleRect = {
                SCREEN_WIDTH - textWidth,
                10,
                textWidth,
                textHeight
            };
            SDL_RenderCopy(renderer, score,NULL, &titleRect);
            SDL_DestroyTexture(score);
        }


        drawStartingShip(x,y,angle,renderer);

        //this will check damage between the ship and asteroids, and the bullets and asteroids
        checkDamage(asteroids, bullets, playerLives, x, y, velocityX, velocityY, running, renderer, angle, playerScore,largeShoot,mediumShoot,
                    smallShoot);

        if(asteroids.size() == 0){
            bullets.clear();
            asteroids.clear();
            level++;
             // Draw Ship (Triangle)
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Ship color (white)
            // Render the remaining lives
            SDL_Texture* levelUp = renderText(renderer, font, "Level Up!", white);
            if (levelUp) {
                int textWidth, textHeight;
                SDL_QueryTexture(levelUp, NULL, NULL, &textWidth, &textHeight);

                SDL_Rect titleRect = {
                    SCREEN_WIDTH / 2 - textWidth / 2,
                    SCREEN_HEIGHT / 2 - textHeight / 2 - 50, // Slightly above center
                    textWidth,
                    textHeight
                };

                SDL_RenderCopy(renderer, levelUp, NULL, &titleRect);
                SDL_DestroyTexture(levelUp); // Free the texture after rendering
            }
            SDL_RenderPresent(renderer);
            SDL_Delay(2);
            int numNewAsteroids;
            // Spawn initial asteroids, needs to be worked on for levels
            if(level < maxLevel){
                int numNewAsteroids = level + 5;
            }
            else{
                numNewAsteroids = numNewAsteroids;
            }
            for (int i = 0; i < numNewAsteroids; ++i) {
                double startX = rand() % SCREEN_WIDTH;
                double startY = rand() % SCREEN_HEIGHT;
                double velocityX = (rand() % (3 + level)) * (rand() % 2 == 0 ? 1 : -1); // Faster asteroids
                double velocityY = (rand() % (3 + level)) * (rand() % 2 == 0 ? 1 : -1);
                int size = 3; // Large asteroid
                asteroids.emplace_back(startX, startY, velocityX, velocityY, size);
            }
        }

        //these for loops render the bullets that are on the screen and the asteroids in the vector
        for (auto& bullet : bullets) bullet.render(renderer);
        for (auto& asteroid : asteroids) asteroid.render(renderer);

        // Update Screen
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // Cap ~60 FPS
    }

  // Cleanup
    TTF_CloseFont(font);
    Mix_CloseAudio();
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return;
}
