// Clase genérica de enemigos (red, pink, cyan, orange)

#ifndef ENEMY_H
#define ENEMY_H

#include "Robot.h"
#include "Player.h"

class Enemy : public Robot
{
public:
    
protected:
    Player *player;
    bool free;
    int direction;

    void checkFreeTiles();
    bool freeTiles[4];    // down, right, up, left
    int lock;

    void findPath(RobotSetpoint targetSetpoint);
    void moveEnemy();
};

#endif