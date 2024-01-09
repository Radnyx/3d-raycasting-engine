#ifndef PLAYER_H
#define PLAYER_H

class Entity;
class Game;

namespace Player
{
    void init(Entity*, Game*);
    void tick(Entity*, Game*);
    void render(Entity*, Game*);
}


#endif // PLAYER_H
