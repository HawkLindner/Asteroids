
#ifndef ASTEROID_H
#define ASTEROID_H

#include <SDL2/SDL.h>
#include <vector>
#include "constants.h"
using namespace std;

class asteroid {
public:
    asteroid(
            double startX,
            double startY, 
            double velX, 
            double velY, 
            int size, 
            int splitCount = 0);
    void update(){
        // Update position
        x += velocityX;
        y += velocityY;

        // Handle screen wrap-around
        wrapAround();
    }                   // Update position
    void render(SDL_Renderer* renderer); // Draw asteroid
    vector<asteroid> split();
    double getX(){
        return this->x;
    };
    double getY(){
        return this->y;
    };
    double getRad(){
        return this->size * 10;
    };
    bool isActive() const{
        return active;
    };           // Check if asteroid is active
    int getSize() const{
        return this->size;
    }            // Return asteroid size
private:
    double x, y;        // Position
    double velocityX, velocityY; // Velocity
    int size;           // Size of the asteroid (e.g., 1 = small, 2 = medium, 3 = large)
    int splitCount;
    bool active;        // Whether the asteroid is still active
    void wrapAround(){
        // Wrap asteroid around the screen edges
        if (x > SCREEN_WIDTH) x = 0;
        if (x < 0) x = SCREEN_WIDTH;
        if (y > SCREEN_HEIGHT) y = 0;
        if (y < 0) y = SCREEN_HEIGHT;
    } // Wrap around the screen edges

};
#endif
