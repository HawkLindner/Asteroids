#ifndef SHARED_CONTEXT_H
#define SHARED_CONTEXT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


/*
    This file creates a global window, renderer, and font for all parts of the game
*/
// Declare shared resources as extern
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* font;

#endif // SHARED_CONTEXT_H
