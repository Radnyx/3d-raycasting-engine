#include "game.h"
#include "render/video.h"
#include "level/level.h"
#include "util/maths.h"
#include "util/input.h"
#include "entity/entity.h"
#include "entity/player.h"
#include "entity/component/wallcollision.h"
#include <SDL2/SDL.h>
#include "render/video.h"

// Note from future: I guess this isn't terrible
#define LOOP_ENTITY_START(name) Entity* name = e_first; while (name != nullptr) {
#define LOOP_ENTITY_END(name) e = e->e_next; }


Game::Game(const std::shared_ptr<Bitmap> screen) : renderEngine(screen)
{
    SDL_SetRelativeMouseMode(SDL_TRUE);
    fov = 0;
    projectionSize = 0;
    centerView = 0;
    e_first = e_last = nullptr;

    setCurrentLevel(new Level("height.gkt"));
    renderEngine.setFogDepth(40);

    // Define the player entity
    Entity* player = (new Entity(Player::init, Player::tick, Player::render))
                            ->addComponent(new Components::WallCollision());
    addEntity(player);

    for (int y = 5; y < 30; y++) {
        for (int x = 5; x < 30; x++) {
            if ((x+2)%3 *2== (y*3)%6)
            renderEngine.addSprite(x, y, 2 + Maths::sin(x + y) * 1, 1, 1);
        }
    }
}


Game::~Game()
{
    while(e_first)
        removeEntity(e_first);
}


/*
================================================================================
    Update the game
================================================================================
*/
void Game::tick()
{
    ticks++;

    // Update each entity
    LOOP_ENTITY_START(e)
        if (e->tick)
            e->tick(e, this);
    LOOP_ENTITY_END(e)
}


/*
================================================================================
    Draw the game
================================================================================
*/
void Game::render(Video * video)
{
    renderEngine.drawScreen(this, video); // Draw walls, initial depth buffer

    LOOP_ENTITY_START(e)
        if (e->render)
            e->render(e, this);
    LOOP_ENTITY_END(e)

    renderEngine.postProcess(this, video);

    video->update();
}


/*
================================================================================
    Set the field of view
================================================================================
*/
Game* Game::setFov(float f)
{
    fov = f;
    projectionSize = Maths::tan(f / 2);
    return this;
}


/*
================================================================================
    Set the location of the camera
================================================================================
*/
void Game::setCameraLocation(float x, float y, float z)
{
    camX = x;
    camY = y;
    camZ = z;
}


/*
================================================================================
    Set the angle the camera is looking at
================================================================================
*/
void Game::setViewAngle(float angle)
{
    viewAngle = angle;
}


/*
================================================================================
    Sets where the center of the view should be angled
        -1 = top of screen
        0 = center
        1 = bottom
================================================================================
*/
void Game::setCenterView(float v)
{
    if (v < -1) v = -1;
    if (v > 1) v = 1;
    centerView = v;
}


/*
================================================================================
    Add an entity to the linked list
================================================================================
*/
void Game::addEntity(Entity* e)
{
    if (e_first == nullptr) // Empty list
    {
        e_first = e;
        e_last = e;
    }
    else
    {
        e_last->e_next = e;
        e->e_prev = e_last;
        e_last = e;
    }

    if (e->init)
        e->init(e, this);
}


/*
================================================================================
    Remove an entity from the linked list
================================================================================
*/
void Game::removeEntity(Entity* e)
{
    if (e == e_first)
    {
        if (e == e_last) // Both first and last
        {
            e_first = nullptr;
            e_last = nullptr;
        }
        else
        {
            e_first = e->e_next;
            e_first->e_prev = nullptr;
        }
    }
    else if (e == e_last)
    {
        e_last = e->e_prev;
        e_last->e_next = nullptr;
    }
    else // In middle of list
    {
        e->e_prev->e_next = e->e_next;
        e->e_next->e_prev = e->e_prev;
    }

    delete e;
}


/*
================================================================================
    Set the current level to play in
================================================================================
*/
void Game::setCurrentLevel(Level* level)
{
    currentLevel = level;
}

