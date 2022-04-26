/**
 * @file Enemy.h
 * @authors CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Enemy robot base class
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef ENEMY_H
#define ENEMY_H

#include "Robot.h"
#include "Player.h"

enum RobotMode
{
    DISPERSION = 1,
    PERSECUTION,
    ESCAPE
};

class Enemy : public Robot
{
public:
    Enemy();

protected:
    Player *player;
    MazePosition initialPosition;
    int lock;

    virtual RobotSetpoint getTargetSetpoint(int levelMode) = 0;
    void setRobotMode(int levelMode);
    int getTimeState();

private:
    std::array<bool, 4> freeTiles; // down, right, up, left

    void update(float deltaTime);
    void checkFreeTiles();
    void move();
    void findPath(RobotSetpoint targetSetpoint);
};

#endif