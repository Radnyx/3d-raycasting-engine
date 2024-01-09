#include "render.h"
#include <math.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <string.h>
#include "raycaster.h"
#include "bitmap.h"
#include "../game.h"
#include "../util/maths.h"
#include "../util/input.h"
#include "../level/level.h"
#include "video.h"


Render::Render(std::shared_ptr<Bitmap> scr) : screen(scr)
{
    depthBuffer = new float[scr->getNumPixels()];
    wallStart = new int32_t[scr->getWidth()];

    // Roughly 5 kB with vertical resolution of 280 pixels
    floorDepth = new yDepth[scr->getHeight()];
    memset(floorDepth, 0, sizeof(yDepth)*scr->getHeight());

    pix = scr->getPixels();
    w = scr->getWidth();
    h = scr->getHeight();
    hw = w / 2;
    hh = h / 2;
    hwd64 = hw * (1 / 64.0f);

    setCeilingHeight(1);
}


Render::~Render()
{
    delete[] depthBuffer;
    delete[] wallStart;
    delete[] floorDepth;
}


/*
================================================================================
    Raycast the walls in vision
================================================================================
*/
void Render::drawScreen(Game* game, Video * video)
{
    // Start off by loading the floor information for each horizontal line
    calcFloorDepth(game);

    Level* level = game->getCurrentLevel();
    int32_t mapWidth = level->getWidth();
    int32_t mapHeight = level->getHeight();

    // Calculate various constants particular to the view
    centerViewhh_hh = game->getCenterView() * hh + hh;
    wp = w * game->getProjectionSize();
    float iw2 = 2.f / w;
    float angle = game->getViewAngle();
    float yd = -Maths::sin(angle);
    float xd = Maths::cos(angle);
    float planeY = xd * game->getProjectionSize();
    float planeX = -yd * game->getProjectionSize();
    float camX = game->getCamX();
    float camY = game->getCamY();
    float camZ = game->getCamZ();

    /* Loop horizontally through view */
    for (int32_t x = 0; x < w; x++) {
        /* Send out ray to search for walls */
        float viewPos = iw2 * x - 1;
        float xRayDir = xd + planeX * viewPos;
        float yRayDir = yd + planeY * viewPos;
        RayCaster rayCaster(camX, camY, xRayDir, yRayDir);

        Tile tile;
        uint8_t highest = 0;
        int16_t firstDraw = h;
        bool found = false;

        // Cast ray until wall or outer boundary is found
        // Note from the future: don't use while(true)
        while(true) {
            // Jump to next block in x direction or y
            rayCaster.step();

            if (rayCaster.isWithinBounds(mapWidth, mapHeight)) {
                tile = level->getTile(rayCaster.getXBlock(), rayCaster.getYBlock());
                if (tile.wallTexture && tile.height > highest) {
                    // Keep track of highest visible so far
                    highest = tile.height;
                    // This wall is worth drawing, keep it
                    WallHit* hit = rayCaster.keepHit();
                    //Calculate where to draw the wall on the screen
                    hit->pixHeight = wp / hit->dist;
                    hit->startDraw = centerViewhh_hh + (camZ - tile.height) * hit->pixHeight;
                    // Don't draw if a taller wall is totally behind the front one
                    if (hit->startDraw < firstDraw) {
                        firstDraw = hit->startDraw;
                    } else {
                        rayCaster.popHit();
                        delete hit;
                    }
                    // Confirmed found a wall
                    found = true;
                    // Have we hit the level limit?
                    if (tile.height == level->getHeighestWall()) {
                        break;
                    }
                }
            }
            else break;

        }

        if (found) {
            // Loop through each visible wall and draw them down the screen
            int32_t yDraw = 0;
            WallHit* hit = rayCaster.popHit(), *next;
            wallStart[x] = hit->startDraw;
            do {
                next = rayCaster.popHit();
                if (yDraw < h) {
                    yDraw = drawVerticalStrip(game, hit, next, x);
                }
                delete hit;
                hit = next;
            } while (next != nullptr);

            // for demonstration: SDL_Delay(20);

            // Draw the floor beneath the wall(s)
            drawFloor(level, x, yDraw, rayCaster.getSide());

        }
        else { // No wall found
           // depthBuffer[x] = 1e11;
        }
    }

    if (level->hasSkyTexture()) {
       drawSkyTexture(game, video); // Draws based on where each wall column starts
    }

    // Aligns sprites from front to back without having to sort
    drawSprites(game, video);
}


/*
================================================================================
    Renders a vertical strip of pixels:
        Ceiling/SkyTexture -> Wall -> Floor
================================================================================
*/
int32_t Render::drawVerticalStrip(Game* game, WallHit* hit, WallHit* next, int32_t xDraw)
{
    Level* level = game->getCurrentLevel();
    Tile& tile = level->getTile(hit->x, hit->y);

    // Calculate distance "fog"
    float depthShade = (fogDepth - hit->dist) / fogDepth;
    if (depthShade < 0) depthShade = 0;
    if (hit->side == 0) depthShade /= 2; // Nice looking shade on certain sides

    float ilh64 = 64.0f / hit->pixHeight; // Inverse-lineHieght * 64

    // Calculate line drawing boundaries given current perspective
    float realstart = hit->startDraw;
    int32_t start = realstart;
    if (start < 0) start = 0;

    int32_t end;
    if (next == nullptr)
        end = start + hit->pixHeight * tile.height - (start - realstart - 2);
    else {
        end = next->startDraw;
    }
    if (end > h) end = h;

    // Draw the ceiling
    int32_t y = start;
    uint32_t* tex = game->getCurrentLevel()->getTexture(tile.wallTexture).getPixels();

    uint32_t textXPix = hit->hPoint;
    // Draw the wall column
    for (; y < end; y++) {
        int32_t textYPix = (int32_t)((y - realstart) * ilh64) & 63; // y-position in texture
        uint32_t col = tex[textXPix + textYPix * 64];
        int32_t pos = xDraw + y * w;
        depthBuffer[pos] = hit->dist;
        // Take individual color channels and darken them
        pix[pos] = ((uint32_t)(((col >> 16) & 0xFF) * depthShade) << 16) |
                   ((uint32_t)(((col >>  8) & 0xFF) * depthShade) <<  8) |
                    (uint32_t)( (col        & 0xFF) * depthShade);
    }

    return y;
}


/*
=================================s===============================================
    Renders the floor tiles in vertical columns
    starting from the bottom of a wall to the bottom of the screen

    TODO?: Drawn pixel buffer for sprites overlapping the floor?
================================================================================
*/
void Render::drawFloor(Level* level, int32_t x, int32_t y, bool adjWallSide)
{
    int32_t holePix = 0;
    bool drawingHole = false;
    int32_t hpKeep;
    float shadeDarken = 1;
    int32_t lastxTile, lastyTile;
    int32_t xtile = -1, ytile = -1;

    for (; y < h; y++) {
        yDepth* data = &floorDepth[y];

        int32_t screenpos = x + data->yw;

        float xd = (x - hw) * yFloorInfo.fovFix;
        if (data->ydLessZero) xd = -xd;
        xd *= data->depth;

        float xz = xd * yFloorInfo.rSin + data->dsCos + yFloorInfo.cx;
        float yz = xd * yFloorInfo.rCos - data->dsSin + yFloorInfo.cy;
        // Keep track of the tile immediately above in screen space
        lastxTile = xtile;
        lastyTile = ytile;
        xtile = (int32_t)(xz) >> 6;
        ytile = (int32_t)(yz) >> 6;
        // Don't bother drawing out of bounds
        if (xtile < 0 || ytile < 0 || xtile >= level->getWidth() || ytile >= level->getHeight())
            continue;
        // Location on texture
        int32_t xx = (int32_t)(xz) & 63;
        int32_t yy = (int32_t)(yz) & 63;

        Tile& tile = level->getTile(xtile, ytile);
        int texture = tile.floorTexture;

        float shade = data->depthShade;

        uint32_t col = 0;
        if (texture == 0) { // Infinite holes
            // Note from the future: I don't think I had the linear algebra skills to do
            //                       this hole effect correctly when I was 18.
            if (!drawingHole) {
                drawingHole = true;
                holePix = 0;
                // Tell how to get the horizontal pixels to draw the
                // texture on the sides of the hole. Also tell which
                // sides to keep darkened as the walls are.
                bool onYSide = (lastyTile == ytile || lastxTile != xtile);
                shadeDarken = shade;
                if (lastxTile != -1) {
                    if (onYSide) {
                        shadeDarken /= 2;
                        hpKeep = yy;
                    } else {
                        hpKeep = xx;
                    }
                } else {
                    hpKeep = xx;
                    if (!adjWallSide) {
                        hpKeep = yy;
                        shadeDarken /= 2;
                    }
                }
            }
            // Calculate vertical pixels by how far down we draw (proportioned accordingly to depth)
            uint32_t yPix = (uint32_t)((holePix + 1) * (floorDepth[y - holePix].depthScale / wp) ) & 63;
            col = level->getTexture(1).getPixels()[hpKeep + yPix * 64];
            holePix++;
            shade = shadeDarken * (1 - (float)holePix / h);
        }
        else { // Regular floor texture
            if (drawingHole) {
                drawingHole = false;
                shadeDarken = 1;
            }
            col = level->getTexture(texture).getPixels()[xx + yy * 64];
        }

        depthBuffer[screenpos] = data->depth * hwd64;
        pix[screenpos] = ((uint32_t)(((col >> 16) & 0xFF) * shade) << 16) |
                         ((uint32_t)(((col >> 8) & 0xFF) * shade) << 8) |
                         (uint32_t)((col & 0xFF) * shade);
    }
}


/*
================================================================================
    Stores depth information
================================================================================
*/
void Render::calcFloorDepth(Game* game)
{
    yFloorInfo.cx = game->getCamX() * 64;
    yFloorInfo.cy = game->getCamY() * 64;
    yFloorInfo.rSin = Maths::sin(game->getViewAngle());
    yFloorInfo.rCos = Maths::cos(game->getViewAngle());
    yFloorInfo.fovFix = game->getProjectionSize();
    float ff64 = yFloorInfo.fovFix * 64;
    float center = hh + game->getCenterView() * hh;
    float fd64 = fogDepth * 64;
    float zz = game->getCamZ(); // Vertical height

    for (int32_t y = 0; y < h; y++) {
        float yd = (y - center);
        bool ydLessZero = yd < 0;
        float depth = (ff64 / yd);

        // Change depth based on vertical position
        if (ydLessZero) depth *= (ceilingHeight - zz) * 2;
        else depth *= (zz) * 2;

        float depthScale = depth * hw;
        if (ydLessZero) depthScale = -depthScale;

        float depthShade = (fd64 - fabs(depth * h)) / (fd64);
        if (depthShade < 0) depthShade = 0;
        // Load all into lookup table
        floorDepth[y].depthScale = depthScale;
        floorDepth[y].depthShade = depthShade;
        floorDepth[y].depth = depth;
        floorDepth[y].ydLessZero = ydLessZero;
        floorDepth[y].yw = y * w;
        floorDepth[y].dsCos = depthScale * yFloorInfo.rCos;
        floorDepth[y].dsSin = depthScale * yFloorInfo.rSin;
    }
}


/*
================================================================================
    Render all sprites scheduled to be drawn
================================================================================
*/
void Render::drawSprites(Game* game, Video * video)
{
    float hfov = game->getFov() / 2;
    float hwdp = hw / game->getProjectionSize();

    // Loop through and render each visible sprite
    for(; sprites.size(); sprites.pop())
    {
        Sprite* spr = &sprites.front();

        float xd = game->getCamX() - spr->x;
        float yd = game->getCamY() - spr->y;
        float angle = atan(-yd/xd);
        float dist = sqrtf(xd * xd + yd * yd);

        // Find lineHeight early, also tells if sprite is behind the camera
        float lineHeight = spr->h * wp / dist;
        if (lineHeight < 0) continue;

        // Calculate darkening
        float depthShade = (fogDepth - dist) / fogDepth;
        if (depthShade < 0) continue; // Completely black, might as well skip

        // This part is pretty borky, but it works...
        if (xd >= 0) angle += Maths::PI;
        else if (angle < -Maths::PI) angle += Maths::PI*2;
        angle = game->getViewAngle() - angle; // Angle relative to screen, 0 being center
        if (angle < -Maths::PI) angle += Maths::PI*2;
        else if (angle > Maths::PI) angle -= Maths::PI*2;

        float cosAngle = Maths::cos(angle);
        dist *= cosAngle; // Fix onto plane

        lineHeight /= cosAngle;

        // Calculate size of sprite
        float yTexScale = 64.f / lineHeight;
        float sw = spr->w * hwdp / dist * cosAngle;
        float hsw = sw / 2;

        float v = angle / hfov; // Converts relative angle to -1 for left end, 1 for right end
        float xa = (v + 1) * hw; // Converts that to actual x-coordinates

        int32_t yfix = (1 + game->getCenterView()) * hh + lineHeight / 2 - (spr->z - game->getCamZ() + .5f) * lineHeight;
        int32_t ystart = 0;
        int32_t yend = lineHeight;
        if (yfix < 0) ystart = -yfix;
        if (yfix + yend >= h) yend = h - yfix;

        float xstart = -hsw;
        if (xa - hsw < 0) xstart = -xa;
        for (int32_t x = xstart; x < hsw; x++)
        {
            int32_t xx = xa + x; // X-coordinate of pixel
            if (xx >= w) break;

            int32_t textureX = ((x + hsw) / sw) * 64;

            // Draw each vertical strip
            for (int y = ystart; y < yend; y++)
            {

                int32_t pos = xx + (yfix + y) * w;
                // Aligns sprites from front to back without having to sort
                if (depthBuffer[pos] < dist) continue;
                depthBuffer[pos] = dist;

                int32_t textureY = y * yTexScale;
                uint32_t col = 0xFF * textureX << textureY;

                // Draw with 0 being bottom
                pix[pos] = ((uint32_t)(((col >> 16) & 0xFF) * depthShade) << 16) |
                         ((uint32_t)(((col >> 8) & 0xFF) * depthShade) << 8) |
                          (uint32_t)((col & 0xFF) * depthShade);
            }
        }

    }
}


/*
================================================================================
    Draw a 360 degree panorama image in the background serving as the sky
================================================================================
*/
void Render::drawSkyTexture(Game* game, Video * video)
{
    Bitmap* texture = game->getCurrentLevel()->getSkyTexture();
    int32_t tw = texture->getWidth();
    int32_t th = texture->getHeight();

    int32_t degrees = Maths::toDegrees(game->getViewAngle());
    int32_t offset = (359 - degrees / 360.f) * tw; // Wrap around horizontally

    int32_t yoff = (game->getCenterView() - 1) * hh;
    int32_t ystart = yoff < 0 ? -yoff : 0;

    float ih = 1.f / h;

    for (int32_t x = 0; x < w; x++)
    {
        for (int32_t y = ystart; y < wallStart[x] - yoff; y++)
        {
            int32_t yy = (y + yoff);
            int32_t pos = x + yy * w;
            depthBuffer[pos] = FAR_DIST;
            pix[pos] = texture->getPixels()[(x + offset) % tw + (uint32_t)((y*ih)*th) * tw];
        }
        wallStart[x] = 0;
    }
}


/*
================================================================================
    Final filter that changes how the game looks
================================================================================
*/
void Render::postProcess(Game* game, Video * video)
{
    float s = 0.8f;
    for (int32_t y = 0; y < h; y++)
    {
        int32_t yw = y * w;
        for (int32_t x = 0; x < w; x++)
        {
            uint32_t col = pix[x + yw];
            uint32_t R = (col >> 16) & 0xFF;
            uint32_t G = (col >> 8) & 0xFF;
            uint32_t B = col & 0xFF;

            if ((((x >> 1) & 1) == 0 && (y & 1) == 0) || (((x >> 1) & 1) == 1 && (y & 1) == 1))
            {
                R *= s;
                G *= s;
                B *= s;
            }
            pix[x + yw] = (R << 16) | (G << 8) | B;

        }
    }
}


/*
================================================================================
    Add a new sprite to be drawn
================================================================================
*/
void Render::addSprite(float x, float y, float z, float w, float h)
{
    Sprite spr = {x, y, z, w, h};
    sprites.push(spr);
}


/*
================================================================================
    Tells how high the ceiling should be drawn
================================================================================
*/
void Render::setCeilingHeight(float h)
{
    ceilingHeight = h;
}


/*
================================================================================
    Sets the distance before shading into darkness
================================================================================
*/
void Render::setFogDepth(float d)
{
    fogDepth = d;
}

float Render::getCeilingHeight() const
{
    return ceilingHeight;
}
