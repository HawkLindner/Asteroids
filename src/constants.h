#ifndef CONSTANTS_H
#define CONSTANTS_H


/*
    This is a shared file amongst all of the game to insure that all constants will be the same throughout all the programs
*/
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;
const double PI = 3.14159265;

inline double toRadians(double degrees) {
    return degrees * PI / 180.0;
}

#endif // CONSTANTS_H
