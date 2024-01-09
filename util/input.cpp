#include "input.h"
#include <SDL2/SDL.h>

namespace Input
{
    int32_t mouseXMove=0, mouseYMove=0;
    bool pressed[256];


    void getMouseMovement(int32_t* xd, int32_t* yd)
    {
        *xd = mouseXMove;
        *yd = mouseYMove;
        mouseXMove = 0;
        mouseYMove = 0;
    }

     /*
    Check for various input events
    */
    void pollInput(bool& running)
    {
        static SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_ESCAPE)
                        running = false;
                    pressed[event.key.keysym.sym & 0xFF] = true;
                break;
                case SDL_KEYUP:
                    pressed[event.key.keysym.sym & 0xFF] = false;
                break;
                case SDL_QUIT:
                    running = false;
                break;
                case SDL_MOUSEMOTION:
                    mouseXMove = event.motion.xrel;
                    mouseYMove = event.motion.yrel;
                break;
            }
        }
    }
}
