#ifndef VIDEO_H
#define VIDEO_H
#include <SDL2/SDL.h>
#include <stdint.h>
#include <iostream>
#include <memory>

class Bitmap;

class Video
{
private:
    SDL_Window* gameWindow;
    SDL_Renderer* renderer;
    SDL_Texture* videoTexture;

public:

    std::shared_ptr<Bitmap> screen;

    Video(const char* title, uint32_t w, uint32_t h, uint32_t scale);
    ~Video();
    void update();
    void destroy();

    int32_t getWidth() const;
    int32_t getHeight() const;
};


#endif // VIDEO_H
