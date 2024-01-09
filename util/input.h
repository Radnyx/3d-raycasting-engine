#ifndef INPUT_H
#define INPUT_H
#include <stdint.h>

namespace Input
{
    extern bool pressed[256];

    void getMouseMovement(int32_t* xd, int32_t* yd);
    void pollInput(bool& running);
}

#endif // INPUT_H
