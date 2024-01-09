#include "bitmap.h"
#include <stdlib.h>
#include "lodepng.h"


Bitmap::Bitmap()
{
    width = 0;
    height = 0;
    numPixels = 0;
}

Bitmap::Bitmap(int32_t w, int32_t h) : numPixels(w * h), pixels(numPixels)
{
    width = w;
    height = h;
    numPixels = w * h;
}


Bitmap::Bitmap(const std::string filename)
{
    unsigned w, h;

    /* The raw pixels (in bytes) */
    std::vector<uint8_t> image;
    /* Decode the image file into the data */
    lodepng::decode(image, w, h, filename);
    width = w;
    height = h;
    numPixels = w * h;
    pixels.resize(numPixels);

    /* Copy raw data into pixels and swap BLUE and RED channels */
    uint8_t* rawpix = (uint8_t*)((void*)&pixels[0]);
    for(uint32_t i = 0; i < image.size(); i++) {
        /* Fix the BGR to RGB issue */
        if(i % 4 == 0) rawpix[i] = image[i+2];
        else if(i % 4 == 2) rawpix[i] = image[i-2];
        else rawpix[i] = image[i];
    }
    /* Clean up */
    image.clear();
}


/*
    Plot a single pixel and check boundaries
*/
void Bitmap::safe_plot(uint32_t color, int32_t x, int32_t y)
{
    if (x < 0 || y < 0 || x >= width || y >= height)
        return;
    pixels[x + y * width] = color;
}

/*
    Fast line drawing,
    "Borrowed" from http://www.edepot.com/linea.html
*/
void Bitmap::drawLine(uint32_t color, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    bool yLonger = false;
	int32_t incrementVal;

	int32_t shortLen = y2 - y1;
	int32_t longLen = x2 - x1;
	if (abs(shortLen) > abs(longLen)) {
		int32_t swp = shortLen;
		shortLen = longLen;
		longLen = swp;
		yLonger = true;
	}

	double divDiff;
	if (shortLen == 0) divDiff = longLen;
	else divDiff = (double)longLen / (double)shortLen;
	if (longLen < 0) incrementVal = -1;
	else incrementVal = 1;
	if (yLonger)
    {
		for (int32_t i = 0; i != longLen;i += incrementVal)
			safe_plot(color, x1 + (int32_t)((double)i / divDiff), y1 + i);
	} else
	{
		for (int32_t i = 0; i != longLen;i += incrementVal)
			safe_plot(color, x1 + i, y1 + (int32_t)((double)i / divDiff));
	}
}

/*
    Clear the bitmap one color
*/
void Bitmap::clear(uint32_t color)
{
    for (int i = 0; i < numPixels; i++)
        pixels[i] = color;
}


uint32_t* Bitmap::getPixels()
{
    return &pixels[0];
}
