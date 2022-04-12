// Clase genérica de enemigos (red, pink, cyan, orange)

#ifndef ENEMY_H
#define ENEMY_H

#include "Robot.h"
#include "Player.h"

class Enemy : public Robot
{
public:

    virtual void start() = 0;

protected:
    Player *player;
    bool free;
};

#endif