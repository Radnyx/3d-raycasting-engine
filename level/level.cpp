#include "level.h"
#include "../render/bitmap.h"
#include <ctime>
#include <fstream>

/* Courtesy of http://www.codecodex.com/wiki/Read_a_file_into_a_byte_array */
char* readFileBytes(const char *name)
{
    std::ifstream fl(name);
    fl.seekg(0, std::ios::end);
    size_t len = fl.tellg();
    char *ret = new char[len];
    fl.seekg(0, std::ios::beg);
    fl.read(ret, len);
    fl.close();
    return ret;
}

const std::string loadString(const uint8_t* data, uint32_t& pos)
{
    std::string str;
    while(data[pos] != 0)
        str += data[pos++];
    pos++;
    return str;
}


Level::Level(std::string file)
{
    skyTexture = new Bitmap("mntn.png");

    uint8_t* data = (uint8_t*)readFileBytes(file.c_str());
    // data[0-2] = "GKT", data[3] = VERSION#
    width = (data[4] << 8) | data[5];
    height = (data[6] << 8) | data[7];
    tiles.resize(width * height);

    uint32_t pos = 8;
    name = loadString(data, pos);

    textures = new Bitmap[256];

    textures[0] = Bitmap(64, 64);
    for (int i = 0; i < 64 * 64; i++) {
        int y = (i / 64) / 8;
        int x = (i % 64) / 8;
        if (x % 2 == y % 2 )
            textures[0].getPixels()[i] = 0xFF00FF;
        else
            textures[0].getPixels()[i] = 0x000000;
    }

    int32_t nText = data[pos++];
    for (int32_t i = 1; i < nText; i++) {
        std::string texfile = "tex/" + loadString(data, pos);
        textures[i] = Bitmap(texfile);
    }

    for (int y = 0; y < height; y++) {
        int32_t yw = y * width;
        for (int x = 0; x < width; x++) {
            Tile& tile = tiles[x + yw];
            tile.floorTexture = data[pos++];
            tile.wallTexture = data[pos++];
            tile.ceilingTexture = data[pos++];
            tile.height = data[pos++];

            if (tile.height > heighestWall)
                heighestWall = tile.height;
        }
    }

    delete[] data;
}

Level::~Level()
{
    if (skyTexture)
        delete skyTexture;
    delete[] textures;
}

/*
    Get the block at the current x and y
*/
Tile& Level::getTile(const int32_t x, const int32_t y)
{
    return tiles[x + y * width];
}

Bitmap& Level::getTexture(const int32_t t)
{
    return textures[t];
}

int32_t Level::getWidth() const
{
    return width;
}

int32_t Level::getHeight() const
{
    return height;
}

Bitmap* Level::getSkyTexture() const
{
    return skyTexture;
}

bool Level::hasSkyTexture() const
{
    return skyTexture != nullptr;
}

uint8_t Level::getHeighestWall() const
{
    return heighestWall;
}
