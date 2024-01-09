#ifndef ENTITY_H
#define ENTITY_H
#include <iostream>
#include <vector>
#include <functional>


class Component;
class Game;

class Entity
{
private:
    // UpDate Function: requires current object and game state
    typedef std::function<void(Entity*, Game*)> UDF;
public:
    Entity* e_prev = nullptr, *e_next = nullptr;

    std::vector<Component*> components;

    float xpos, ypos, zpos; // Location
    float xvel, yvel, zvel; // Velocity
    float accelSpeed; // Acceleration speed
    float angle; // Angle facing
    bool isMoving; // Test if the entity is moving

    float width, height; // Physical width and height

    UDF init, tick, render; // General update functions, init is called on creation

    Entity(UDF i, UDF t, UDF r);
    ~Entity();

    Entity* addComponent(Component*);

    void updateMovement();
    void updateComponents(Game*);
};

#endif // ENTITY_H
