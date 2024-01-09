#ifndef LEVEL_H
#define LEVEL_H
#include <stdint.h>
#include <iostream>
#include <vector>
#include "tile.h"

class Bitmap;

class Level
{
private:
    std::string name;
    int32_t width, height;
    uint8_t heighestWall;

    /**
        @todo Blocks are simply 8-bit integers right now.
            This could define their properties with certain flags.
            However I plan to use specific block objects in the future
            which certain information, including:

                - If a block should have a different texture for a particular side
                - If the block is solid
                - Transparency?
                - Causes an event on use
                - Flipped horizontally/vertically

        6/26/2016 - Changed blocks to "tiles"
    */
    std::vector<Tile> tiles;
    Bitmap* skyTexture = nullptr;
    Bitmap* textures;

public:

    Level(const std::string file);
    ~Level();

    Tile& getTile(const int32_t x, const int32_t y);
    Bitmap& getTexture(const int32_t t);
    int32_t getWidth() const;
    int32_t getHeight() const;

    Bitmap* getSkyTexture() const;
    bool hasSkyTexture() const;
    uint8_t getHeighestWall() const;

};


#endif // LEVEL_H
