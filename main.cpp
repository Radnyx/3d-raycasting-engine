#undef _GLIBCXX_USE_WCHAR_T

#include <iostream>
#include "game.h"
#include "util/maths.h"
#include "util/input.h"
#include "render/video.h"
#include "render/bitmap.h"

Game* game; // Game manager
Video* video; // Screen information

// Is the game running?
bool running = true;

/*
    Update game 60 times per second
*/
void tick()
{
    Input::pollInput(running);
    game->tick();
}

/*
    Update the screen
*/
void render()
{
    game->render(video);
}

/*
    Main loop execution and control timing
*/
void mainLoop()
{
    static uint32_t ticks = 0;
    static uint64_t fpstime = 0;
    static uint64_t starttime = SDL_GetTicks();

    uint64_t gettime = SDL_GetTicks();
    if((gettime - starttime) >= 16) // Update 60fps
    {
        tick(); // Update game
        render(); // Draw game
        starttime += 16;
        ticks++;
    }

    // Simply print out the FPS
    if((gettime - fpstime) >= 1000)
    {
        std::cout << ticks << std::endl;
        ticks = 0;
        fpstime = SDL_GetTicks();
    }

}

int main(int argc, char* argv[])
{
    // Generate math trig tables
    Maths::genTables();

    // Initialize video/drawing data
    SDL_Init(SDL_INIT_EVERYTHING);
    video = new Video("Ray Caster Test Pre-Alpha 0.0.9", 640, 360, 2);

    // Setup game, FOV of 70 degrees
    game = (new Game(video->screen))->setFov(Maths::toRadians(70));

    while(running)
    {
        mainLoop();
    }

    delete game;
    return 0;
}
