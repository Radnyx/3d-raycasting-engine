#include "maths.h"
#include <math.h>
#include <stdint.h>

namespace Maths
{
    #define TABLE_DENSITY 2000
    float SINE[TABLE_DENSITY];
    float COSINE[TABLE_DENSITY];
    float TAN[TABLE_DENSITY/2];

    /*
        Generate sine and cosine tables
    */
    void genTables()
    {
        for (uint32_t i = 0; i < TABLE_DENSITY; i++)
            SINE[i] = sinf((i / (float)TABLE_DENSITY) * PI * 2),
            COSINE[i] = cosf((i / (float)TABLE_DENSITY) * PI * 2);
        for (uint32_t i = 0; i < TABLE_DENSITY / 2; i++)
            TAN[i] = SINE[i]/COSINE[i];
    }

    /*
        Access the (co)sine table given an angle
    */
    int intable(float value)
    {
        value += PI * 2;
        return (int)(((value) / (PI * 2)) * (float)TABLE_DENSITY) % TABLE_DENSITY;
    }

    float sin(float f)
    {
        return SINE[intable(f)];
    }

    float cos(float f)
    {
        return COSINE[intable(f)];
    }

    float tan(float f)
    {
        f += PI;
        return TAN[(int)((f / (PI)) * (float)(TABLE_DENSITY / 2)) % (TABLE_DENSITY / 2)];
    }

    int toDegrees(float f)
    {
        f /= PI;
        f *= 180;
        int d = (int)f;
        if (d >= 0) return d % 360;
        else return 360 - ((-d) % 360);
    }

    float toRadians(float f)
    {
        return f / 180.f * PI;
    }

}
