#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h> // Include SDL_ttf for TTF_Font
#include <string>         // Include string for std::string
#include "bullet.h"
#include "asteroid.h"
void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color);

void drawStartingShip(double x, double y, double angle,SDL_Renderer* renderer);
void checkDamage(vector<asteroid> &asteroids, vector<bullet> &bullets,int &playerLives, double x, double y,double velocityX, double velocityY, bool &gameOver
                ,SDL_Renderer* rendnerer, double angle);
// Function to start the game loop
void createGame();

#endif
