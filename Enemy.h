// Clase genérica de enemigos (red, pink, cyan, orange)

#ifndef ENEMY_H
#define ENEMY_H

#include "Robot.h"
#include "Player.h"

#define DISPERSION 1
#define PERSECUTION 2

class Enemy : public Robot
{
public:
    
protected:
    Player *player;
    bool free;
    int direction;
    float time;
    int mode;

    void setTime();
    int getTimeState();

    bool freeTiles[4] = { 0.0f, 0.0f, 0.0f, 0.0f };    // down, right, up, left
    void checkFreeTiles();

    int lock = 0;
    void findPath(RobotSetpoint targetSetpoint);
    void moveEnemy();
};

#endif