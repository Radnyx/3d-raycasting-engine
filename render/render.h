#ifndef RENDER_H
#define RENDER_H
#include <iostream>
#include <memory>
#include <queue>


class Game;
class Bitmap;
class Level;
class RayCaster;
class Tile;
class WallHit;
class Video;


/*
    Information storing how a specific sprite should be drawn
*/
struct Sprite
{
    float x, y, z; // Coordinates
    float w, h; // Size (not texture, scaled to world)
};


/*
    Renders a 3D-like scene with special properties:
        Boxed walls
        Flat floor and ceiling
        Sprites
        Background texture
*/
class Render
{
private:
    const float FAR_DIST = 1e11;

    // Structure to store the current frames vertical floor information
    // to be used after drawing the walls
    struct {
        // Constant per-frame given current perspective
        float cx, cy;
        float rSin, rCos;
        float fovFix;
    } yFloorInfo;

    // Depth calculations
    struct yDepth {
        bool ydLessZero;
        float depth;
        float depthScale;
        float depthShade;
        float yw;
        float dsCos, dsSin;
    } *floorDepth;


    float wp; // Width * game->Projection Size
    float hwd64; // half width / 64
    float _ih64; // 1 - 1/height * 64
    float centerViewhh_hh; // game->Center View * hh + hh;

    uint32_t* pix;
    int32_t w, h;
    int32_t hw, hh;

    std::queue<Sprite> sprites; // List of sprites to be drawn
    float* depthBuffer; // Measures distance to each vertical wall strip

    // Keep track of where vetically each drawn wall starts
    // Eliminates overdraw when rendering the sky texture
    int32_t* wallStart;

    float ceilingHeight; // 1 is normal, 2 is 2 blocks high
    float fogDepth;

    std::shared_ptr<Bitmap> screen;

    void calcFloorDepth(Game* game);
    int32_t drawVerticalStrip(Game* game, WallHit* hit, WallHit* next,
                           int32_t xDraw);
    void drawFloor(Level* level, int32_t x, int32_t y, bool adjWallSide);
    void drawSkyTexture(Game* game, Video * video);

public:
    Render(std::shared_ptr<Bitmap> screen);
    ~Render();

    void drawSprites(Game* game, Video *);
    void drawScreen(Game* game, Video * video);
    void postProcess(Game* game, Video *);

    void addSprite(float x, float y, float z, float w, float h);
    void setCeilingHeight(float h);
    void setFogDepth(float d);

    float getCeilingHeight() const;
};

#endif // RENDER_H
