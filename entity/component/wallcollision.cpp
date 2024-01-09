#include "wallcollision.h"
#include "../entity.h"
#include "../../game.h"
#include "../../level/level.h"
#include <math.h>

struct Box
{
    float x, y, width, height;
};

bool intersects(Box a, Box b) {
    return (fabs(a.x - b.x) * 2 < (a.width + b.width)) &&
         (fabs(a.y - b.y) * 2 < (a.height + b.height));
}


namespace Components
{
    WallCollision::WallCollision(){}
    WallCollision::~WallCollision(){}


    /*
        Prevent the entity from moving through walls
    */
    void WallCollision::tick(Entity* entity, Game* game)
    {
        float w = entity->width;
        float hw = w / 2;
        float xp = entity->xpos;
        float yp = entity->ypos;
        Level* level = game->getCurrentLevel();

        // Loop through nearby blocks based on entity size
        int32_t yend = ceilf(yp + w);
        int32_t xend = ceilf(xp + w);
        for (int32_t y = (int32_t)(yp - w); y < yend; y++)
        {
            for (int32_t x = (int32_t)(xp - w); x < xend; x++)
            {
                int32_t block = 0;
                if (x < 0 || y < 0 || x >= level->getWidth() || y >= level->getHeight())
                    block = 1;
                else
                {
                    Tile& tile = level->getTile(x, y);
                    block = tile.wallTexture || !tile.floorTexture;
                }
                if (block) // Block is near
                {
                    // Compare each side with the block, then snap
                    // the axis being tested onto the block and
                    // zero the velocity in that direction
                    Box blockBox = {(float)x+.5f, (float)y+.5f, 1.f, 1.f};
                    Box eBoxL = {xp-w/4, yp, hw, (hw+w)/2.f}; // Notice each side box
                    Box eBoxR = {xp+w/4, yp, hw, (hw+w)/2.f}; // is slightly smaller so that they
                    Box eBoxU = {xp, yp-w/4, (hw+w)/2.f, hw}; // don't intersect and throw the
                    Box eBoxD = {xp, yp+w/4, (hw+w)/2.f, hw}; // entity a mile away

                    //bool ltrig = 0, rtrig = 0, utrig = 0, dtrig = 0;

                    // unit tests?
                    if (intersects(blockBox, eBoxL) && entity->xvel < 0)
                    {
                        entity->xpos = x + 1 + hw;
                        entity->xvel = 0;//-entity->xvel * 0.5;
                    }
                    if (intersects(blockBox, eBoxU) && entity->yvel < 0)
                    {
                        entity->ypos = y + 1 + hw;
                        entity->yvel = 0;
                    }
                    if (intersects(blockBox, eBoxR) && entity->xvel > 0)
                    {
                        entity->xpos = x - hw;
                        entity->xvel = 0;
                    }
                    if (intersects(blockBox, eBoxD) && entity->yvel > 0)
                    {
                        entity->ypos = y - hw;
                        entity->yvel = 0;
                    }

                    //if (ltrig && dtrig) {

                    //}

                }
            }
        }
    }
}
