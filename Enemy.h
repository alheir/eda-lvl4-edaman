// Clase genérica de enemigos (red, pink, cyan, orange)

#ifndef ENEMY_H
#define ENEMY_H

#include "Robot.h"
#include "Player.h"

#define DISPERSION 1
#define PERSECUTION 2
#define ESCAPE 3

class Enemy : public Robot
{
public:
    
protected:
    Player *player;
    bool free;
    int direction;
    float time;
    int levelState;

    void resetTime();
    int getTimeState();

    bool freeTiles[4] = {false, false, false, false};    // down, right, up, left
    void checkFreeTiles();

    int lock = 0;
    void update(float deltaTime);
    void setRobotMode(int levelMode);

    RobotSetpoint getTargetSetpoint(int levelMode);

private:
    void moveEnemy();
    void findPath(RobotSetpoint targetSetpoint);
};

#endif