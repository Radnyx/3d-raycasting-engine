#include "raycaster.h"
#include <math.h>


/*
================================================================================
    Initialize a new Ray Caster given the directions
================================================================================
*/
RayCaster::RayCaster(float camX, float camY, float xRayDir, float yRayDir)
{
    xRay.location = camX;
    xRay.dir = xRayDir;
    yRay.location = camY;
    yRay.dir = yRayDir;

    float yrd2 = yRayDir * yRayDir;
    float xrd2 = xRayDir * xRayDir;
    xRay.jumpDist = sqrtf(1 + (yrd2) / (xrd2));
    yRay.jumpDist = sqrtf(1 + (xrd2) / (yrd2));

    xBlock = (int32_t)(camX);
    yBlock = (int32_t)(camY);

    init(xRay);
    init(yRay);
}


/*
================================================================================
    Sets up the individual Ray components for step distances
================================================================================
*/
void RayCaster::init(RayInfo& ray)
{
    if (ray.dir < 0) {
        ray.step = -1;
        ray.sideDist = (ray.location - (int32_t)(ray.location)) * ray.jumpDist;
    }
    else {
        ray.step = 1;
        ray.sideDist = ((int32_t)(ray.location) + 1 - ray.location) * ray.jumpDist;
    }
}


/*
================================================================================
    Jump the Ray to the nearest wall side, either x or y
================================================================================
*/
void RayCaster::step()
{
    if (xRay.sideDist < yRay.sideDist) {
        xRay.sideDist += xRay.jumpDist;
        xBlock += xRay.step;
        side = 0;
    }
    else {
        yRay.sideDist += yRay.jumpDist;
        yBlock += yRay.step;
        side = 1;
    }
}



/*
================================================================================
    Keeps the currently encountered wall in mind to add to the drawing
================================================================================
*/
WallHit* RayCaster::keepHit()
{
    WallHit* hit = new WallHit;
    hit->x = xBlock;
    hit->y = yBlock;
    hit->side = side;
    processOnFind(hit->dist, hit->hPoint);
    hits.push(hit);
    return hit;
}


/*
================================================================================
    Return the wall-hit with the highest priority to be drawn first
================================================================================
*/
WallHit* RayCaster::popHit()
{
    if (!hits.size()) return nullptr;
    WallHit* hit = hits.top();
    hits.pop();
    return hit;
}


/*
================================================================================
    Tell if there are still wall-hits to process and draw
================================================================================
*/
bool RayCaster::hasHits()
{
    return hits.size();
}


/*
================================================================================
    If a wall was found, calculate certain data like distance to wall,
    and the horizontal point hit
================================================================================
*/
void RayCaster::processOnFind(float& wallDist, uint8_t& hPoint)
{
    float h;
    if (side == 0) {
        wallDist = (xBlock - xRay.location + (1 - xRay.step) / 2.f) / xRay.dir;
        h = yRay.location + wallDist * yRay.dir;
    }
    else {
        wallDist = (yBlock - yRay.location + (1 - yRay.step) / 2.f) / yRay.dir;
        h = xRay.location + wallDist * xRay.dir;
    }
    h -= floor(h);
    hPoint = (uint8_t)(h * 64);
}

/*
================================================================================
    Tells if the ray is still whithin the map
================================================================================
*/
bool RayCaster::isWithinBounds(int32_t w, int32_t h)
{
    return xBlock >= 0 && yBlock >= 0 && xBlock < w && yBlock < h;
}
