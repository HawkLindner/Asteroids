/*
    Represents asteroids.
    Handles asteroid movement and splitting when destroyed.
*/

#include "asteroid.h"
#include "constants.h"
#include <iostream>
#include <vector>

using std::cout;
using std::endl;


asteroid::asteroid(double startX, double startY, double velX, double velY, int size, int splitCount)
    :x(startX), y(startY), velocityX(velX), velocityY(velY), size(size), 
    splitCount(splitCount), active(true) {}


vector<asteroid> asteroid::split() {
    vector<asteroid> fragments;

    if (splitCount >= 2 || size <= 1) {
            active = false; // Destroy the asteroid
        } else {
            // Split into two smaller asteroids
            int newSize = size - 1; // Reduce size
            if(newSize < 1)newSize =1;
            double newVelX1 = velocityX + (rand() % 3 - 1);
            double newVelY1 = velocityY + (rand() % 3 - 1);
            double newVelX2 = -velocityX + (rand() % 3 - 1);
            double newVelY2 = -velocityY + (rand() % 3 - 1);
            fragments.emplace_back(x, y, newVelX1, newVelY1, newSize, splitCount + 1);
            fragments.emplace_back(x, y, newVelX2, newVelY2, newSize, splitCount + 1);
            active = false; // Mark the original asteroid as inactive
        }
    return fragments;
}

void asteroid::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for asteroids

    // Draw asteroid as a circle (simple approximation using SDL_Rect for now)
    SDL_Rect rect = { static_cast<int>(x - size * 10), static_cast<int>(y - size * 10),
                      size * 20, size * 20 };
    SDL_RenderDrawRect(renderer, &rect);
}
