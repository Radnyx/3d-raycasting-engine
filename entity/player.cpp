#include "player.h"
#include "entity.h"
#include "../game.h"
#include "../util/maths.h"
#include "../util/input.h"
#include <stdint.h>
#include <math.h>

namespace Player
{

    float center = 0.0f; // Where the view is looking vertically
    float headBob = 0.0f; // Distance from 'center' view to bounce
    uint32_t moveTicks = 0; // Ticks since begun moving (to count headBob)

    /*
        Control the looking of the player
    */
    void controlLook(Entity* p)
    {
        int32_t mouseX, mouseY;
        Input::getMouseMovement(&mouseX, &mouseY);
        // Move mouse horizontally
        p->angle -= mouseX / 100.f;
        if (p->angle > Maths::PI_2) p->angle -= Maths::PI_2;
        if (p->angle < 0) p->angle += Maths::PI_2;
        // Move mouse vertically
        center -= mouseY / 58.f;
        if (center < -0.8f) center = -0.8f;
        if (center > 0.8f) center = 0.8f;
    }

    /*
        Control the movement of the player
    */
    void controlMove(Entity* p)
    {
        float speed = p->accelSpeed;
        float angle = p->angle;

        bool pressup = Input::pressed['w'];
        bool pressdown = Input::pressed['s'];
        bool pressright = Input::pressed['d'];
        bool pressleft = Input::pressed['a'];

        p->isMoving = ((pressup ^ pressdown) || (pressright ^ pressleft));
        if (p->isMoving)
        {
            moveTicks++;
            headBob = Maths::sin(moveTicks / 6.f) / 32.f;
        }
        else // Once no longer moving, dampen headBob to normal
        {
            moveTicks = 0;
            headBob *= 0.9f;
        }

        // Prevent from moving faster diagonally
        if ((pressup || pressdown) && (pressleft || pressright))
            speed *= Maths::HSQRT_2;
        // Move on key press without opposite
        if (pressup && !pressdown)
        {
            p->yvel += -Maths::sin(angle) * speed;
            p->xvel += Maths::cos(angle) * speed;
        }
        if (pressdown && !pressup)
        {
            p->yvel += Maths::sin(angle) * speed;
            p->xvel += -Maths::cos(angle) * speed;
        }
        if (pressright && !pressleft)
        {
            p->yvel += -Maths::sin(angle - Maths::PI / 2) * speed;
            p->xvel += Maths::cos(angle - Maths::PI / 2) * speed;
        }
        if (pressleft && !pressright)
        {
            p->yvel += -Maths::sin(angle + Maths::PI / 2) * speed;
            p->xvel += Maths::cos(angle + Maths::PI / 2) * speed;
        }
    }

    /*
        Control the acceleration of the player as it moves
    */
    void controlSpeed(Entity* p)
    {
        // Dampen movement (simulates friction
        p->xvel *= 0.9f;
        p->yvel *= 0.9f;
    }

    /*
        Initialize the player
    */
    void init(Entity* p, Game* game)
    {
        p->width = 0.5f;
        p->height = 0.5f;
        p->xpos = 16.5f;
        p->ypos = 27.5f;
        p->zpos = 0.5f;
        p->xvel = 0;
        p->yvel = 0;
        p->zvel = 0;
        p->angle = 5 * -Maths::PI / 4;
        p->accelSpeed = 0.008f;
        p->isMoving = false;
    }


    void tick(Entity* player, Game* game)
    {
        controlLook(player);
        controlMove(player);
        controlSpeed(player);

        // Update game view based on player information
        game->setCameraLocation(player->xpos, player->ypos, player->zpos + headBob);
        game->setViewAngle(player->angle);
        game->setCenterView(center + headBob);

        player->updateMovement();
        player->updateComponents(game);

        //float netspeed = sqrtf(player->xvel*player->xvel + player->yvel*player->yvel);
        //std::cout << netspeed << std::endl;
    }

    void render(Entity* player, Game* game)
    {

    }

};
