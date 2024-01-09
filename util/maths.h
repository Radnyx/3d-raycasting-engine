#ifndef MATHS_H
#define MATHS_H




/*
    Contains sine tables to reduce real-time computation
*/
namespace Maths
{
    const float PI = 3.14159265359f;
    const float PI_2 = 6.28318530718f;
    const float SQRT_2 = 1.41421356237f;
    const float HSQRT_2 = 0.70710678118f;

    void genTables();
    float sin(float);
    float cos(float);
    float tan(float);
    int toDegrees(float); // Approximate degree 0->360
    float toRadians(float);
};


#endif // MATHS_H
