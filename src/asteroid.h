#ifndef ASTEROID_H
#define ASTEROID_H

#include <SDL2/SDL.h>
#include <vector>
using namespace std;

class asteroid {
public:
    asteroid(double startX, double startY, double velX, double velY, int size, int splitCount = 0);


    void update();                   // Update position
    void render(SDL_Renderer* renderer); // Draw asteroid
    bool isActive() const;           // Check if asteroid is active

    int getSize() const;             // Return asteroid size

    vector<asteroid> split();
    double getX();
    double getY();
    double getRad();
private:
    double x, y;        // Position
    double velocityX, velocityY; // Velocity
    int size;           // Size of the asteroid (e.g., 1 = small, 2 = medium, 3 = large)
    int splitCount;
    bool active;        // Whether the asteroid is still active

    void wrapAround();  // Wrap around the screen edges
};

#endif
