#ifndef TILE_H
#define TILE_H
#include <stdint.h>

/*
    This represents one unit of map space that
    includes a floor, wall, and ceiling.
*/
struct Tile
{
    uint8_t floorTexture, wallTexture, ceilingTexture;
    float height;
};

#endif // TILE_H
