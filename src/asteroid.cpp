/*
    Represents asteroids.
    Handles asteroid movement and splitting when destroyed.
*/

#include "asteroid.h"
#include <vector>

using namespace std;

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 900;

asteroid::asteroid(double startX, double startY, double velX, double velY, int size, int splitCount)
    : x(startX), y(startY), velocityX(velX), velocityY(velY), size(size), 
    splitCount(splitCount), active(true) {}

void asteroid::update() {
    // Update position
    x += velocityX;
    y += velocityY;

    // Handle screen wrap-around
    wrapAround();
}
vector<asteroid> asteroid::split() {
    vector<asteroid> fragments;

if (splitCount >= 2 || size <= 1) {
        active = false; // Destroy the asteroid
    } else {
        // Split into two smaller asteroids
        int newSize = size / 2; // Reduce size
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

double asteroid::getX(){
    return x;
}
double asteroid::getY(){
    return y;
}
double asteroid::getRad(){
    return size * 10;
}
void asteroid::wrapAround() {
    // Wrap asteroid around the screen edges
    if (x > SCREEN_WIDTH) x = 0;
    if (x < 0) x = SCREEN_WIDTH;
    if (y > SCREEN_HEIGHT) y = 0;
    if (y < 0) y = SCREEN_HEIGHT;
}

bool asteroid::isActive() const {
    return active;
}

int asteroid::getSize() const {
    return size;
}
