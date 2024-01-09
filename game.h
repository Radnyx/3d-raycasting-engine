#ifndef GAME_H
#define GAME_H
#include <stdint.h>
#include <iostream>
#include <memory>
#include "render/render.h"

class Entity;
class Level;
class Bitmap;
class Video;

class Game
{
private:
    // Linked-list of entities
    Entity* e_first, *e_last;

    Level* currentLevel;

    Render renderEngine;

    // X and Y are horizontal, Z is vertical
    float camX, camY, camZ;
    float viewAngle;

    // Relative distance of pitch, 1 is top of screen, -1 is bottom
    float centerView;
    // Field of vision
    float fov;
    // This is the size of the projection plane
    // as compared to the distance to it
    float projectionSize;

    // Tracks number of times "tick" has been called
    uint32_t ticks;

public:

    explicit Game(const std::shared_ptr<Bitmap>);
    ~Game();

    void tick();
    void render(Video * video);

    Game* setFov(float fov);
    void setCameraLocation(float x, float y, float z);
    void setViewAngle(float angle);
    void setCenterView(float v);

    void addEntity(Entity*);
    void removeEntity(Entity*);
    void setCurrentLevel(Level*);
    inline Level* getCurrentLevel() const { return currentLevel; }

    inline float getFov() const { return fov; }
    inline float getProjectionSize() const { return projectionSize; }
    inline float getCamX() const { return camX; }
    inline float getCamY() const { return camY; }
    inline float getCamZ() const { return camZ; }
    inline float getViewAngle() const { return viewAngle; }
    inline float getCenterView() const { return centerView; }
    inline uint32_t getTicks() const { return ticks; }

};


#endif // GAME_H
