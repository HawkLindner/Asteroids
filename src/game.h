#ifndef GAME_H
#define GAME_H

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
SDL_Texture* renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color);
void renderTextOnScreen(SDL_Renderer* renderer, SDL_Texture* textTexture, int x, int y);
void drawStartingShip(double x, double y, double angle,SDL_Renderer* renderer);
void checkDamage(vector<asteroid> &asteroids, vector<bullet> &bullets,int &playerLives, double x, double y,double velocityX, double velocityY, bool &gameOver
                ,SDL_Renderer* rendnerer, double angle);
// Function to start the game loop
void createGame();
int getLives();



#endif
