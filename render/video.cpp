#include "video.h"
#include "bitmap.h"


Video::Video(const char* title, uint32_t w, uint32_t h, uint32_t scale)
: screen(std::make_shared<Bitmap>(w, h))
{


    gameWindow = SDL_CreateWindow(title,
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                   w * scale, h * scale, 0);

    renderer = SDL_CreateRenderer(gameWindow, -1, 0);


    // Scale the SDL window
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_RenderSetLogicalSize(renderer, w, h);

    videoTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        w, h);
}

Video::~Video()
{
    destroy();
}

void Video::update()
{
    SDL_UpdateTexture(videoTexture, 0, screen->getPixels(), screen->getWidth()*sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, videoTexture, 0, 0);
    SDL_RenderPresent(renderer);
}

void Video::destroy()
{
    SDL_DestroyWindow(gameWindow);
    SDL_DestroyTexture(videoTexture);
    SDL_DestroyRenderer(renderer);
}

int32_t Video::getWidth() const
{
    return screen->getWidth();
}

int32_t Video::getHeight() const
{
    return screen->getHeight();
}
