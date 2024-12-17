
#include "bullet.h"
#include <cmath>


/*
    Represents bullets fired by the ship.
    Updates bullet positions and handles out-of-bounds checks.
*/

const double PI = 3.14159265;
inline double toRadians(double degrees) { return degrees * PI / 180.0; }

double bullet::getX(){
    return x;
}
double bullet::getY(){
    return y;
}
bullet::bullet(double startX, double startY, double angle) 
    : x(startX), y(startY), active(true) {
    // Calculate bullet velocity based on angle
    velocityX = cos(toRadians(angle)) * speed;
    velocityY = sin(toRadians(angle)) * speed;
}

void bullet::update(){
    x += velocityX;
    y += velocityY;

    if (x < 0 || x > 1200 || y < 0 || y > 800) {
        active = false; // Mark as inactive
    }
}

void bullet::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Bullet color: white
    SDL_Rect rect = { static_cast<int>(x - 2), static_cast<int>(y - 2), 4, 4 }; // Small square for bullet
    SDL_RenderFillRect(renderer, &rect);
}

bool bullet::isActive() const {
    return active;
}