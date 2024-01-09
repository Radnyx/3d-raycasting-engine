#ifndef COMPONENT_H
#define COMPONENT_H

class Entity;
class Game;

class Component
{
private:
public:
    Component();
    virtual ~Component();
    virtual void tick(Entity* entity, Game* game);
};

#endif // COMPONENT_H
