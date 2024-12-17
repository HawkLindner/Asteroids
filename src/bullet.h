#ifndef BULLET_H
#define BULLET_H

#include <SDL2/SDL.h>

class bullet {
public:
    bullet(double startX, double startY, double angle);

    // Move constructor
    bullet(bullet&& other) noexcept
        : x(other.x), y(other.y), velocityX(other.velocityX), velocityY(other.velocityY), 
          active(other.active), speed(other.speed) {}

    // Move assignment operator
    bullet& operator=(bullet&& other) noexcept {
        if (this != &other) {
            x = other.x;
            y = other.y;
            velocityX = other.velocityX;
            velocityY = other.velocityY;
            active = other.active;
            // No need to assign "speed" because it's const
        }
        return *this;
    }

    void update();
    void render(SDL_Renderer* renderer);
    bool isActive() const;
    double getX();
    double getY();

private:
    double x, y;
    double velocityX, velocityY;
    bool active;

    const double speed = 10.0; // Keep "const" here
};

#endif