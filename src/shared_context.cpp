#include "shared_context.h"

/*
    This program declares all the variables at the start of the program. Each part of the game will create and assign new meanings to
    these variables.
*/
// Define shared resources
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;
