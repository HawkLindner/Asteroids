#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <string>

class text{
    public:
        text(const std::string &font, int font_size, const std::string &message, const SDL_Color &color);

        void display() const;

        static SDL_Texture *loadFont();
    private:
        SDL_Texture *text_texture = nullptr;
        SDL_Rect text_rect;

};