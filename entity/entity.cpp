#include "entity.h"
#include <string.h>
#include "component/component.h"

Entity::Entity(UDF i, UDF t, UDF r)
{
    init = i;
    tick = t;
    render = r;
}

Entity::~Entity()
{
    // Clear components
    while(components.size())
    {
        delete components.back();
        components.pop_back();
    }
}

/*
    Adds certain behavior to an entity
*/
Entity* Entity::addComponent(Component* comp)
{
    components.push_back(comp);
    return this;
}

void Entity::updateMovement()
{
    xpos += xvel;
    ypos += yvel;
    zpos += zvel;
}

void Entity::updateComponents(Game* game)
{
    for(Component* c : components)
        c->tick(this, game);
}
