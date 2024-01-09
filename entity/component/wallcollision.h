#ifndef WALL_COLLISION_H
#define WALL_COLLISION_H
#include "component.h"



namespace Components
{
    class WallCollision : public Component
    {
    private:
    public:
        WallCollision();
        ~WallCollision();
        void tick(Entity* entity, Game* game);
    };
}

#endif // WALL_COLLISION_H
