#ifndef RAYCASTER_H
#define RAYCASTER_H
#include <stdint.h>
#include <iostream>
#include <stack>


/*
    Defines where a wall was hit and where it should be drawn
*/
struct WallHit {
    uint16_t x, y; // Map location
    uint8_t hPoint; // Horizontal (texture) point hit
    float dist; // Distance away from hit
    float pixHeight; // Height in pixels
    int16_t startDraw; // Starting pixel to draw (scale is given when drawing)
    bool side; // X or Y side hit (NS, EW)
};


/*
    Handles all ray-casting related math and updating, once
    it hits a wall it will log all visible walls from a given
    ray. (If some are taller and behind the front wall for example)
*/
class RayCaster
{
private:
    struct RayInfo
    {
        float location;
        float dir;
        float step;
        float jumpDist;
        // Length of ray from current position to next x or y side
        float sideDist;
    } xRay, yRay;

    // Note from future: I can't believe I'm allocating memory for these hits every frame.
    //                   There is no way this is necessary.
    std::stack<WallHit*> hits;

    int32_t xBlock, yBlock;
    bool side;

    void init(RayInfo& ray);
    void processOnFind(float& wallDist, uint8_t& hPoint);

public:
    RayCaster(float camX, float camY, float xRayDir, float yRayDir);

    void step();
    WallHit* keepHit();
    WallHit* popHit();

    bool hasHits();
    bool isWithinBounds(int32_t w, int32_t h);
    inline int32_t getXBlock() const { return xBlock; }
    inline int32_t getYBlock() const { return yBlock; }
    inline float getXDir() const { return xRay.dir; }
    inline float getYDir() const { return yRay.dir; }
    inline bool getSide() const { return side; }
};


#endif // RAYCASTER_H
