#ifndef BITMAP_H
#define BITMAP_H
#include <stdint.h>
#include <iostream>
#include <memory>
#include <vector>

class Bitmap
{
private:
    int32_t numPixels;
    int32_t width, height;
    std::vector<uint32_t> pixels;

public:
    Bitmap();
    Bitmap(int32_t width, int32_t height);
    Bitmap(const std::string filename);

    void safe_plot(uint32_t color, int32_t x, int32_t y);
    void drawLine(uint32_t color, int32_t x1, int32_t y1, int32_t x2, int32_t y2);
    void clear(uint32_t col);

    uint32_t* getPixels();
    inline int32_t getNumPixels() const { return numPixels; }
    inline int32_t getWidth() const { return width; }
    inline int32_t getHeight() const { return height; }
};


#endif // BITMAP_H
