/**
 * @file Enemy.h
 * @authors CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Clase base de robots enemigos
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
#include <array>

#define DISPERSION 1
#define PERSECUTION 2
#define ESCAPE 3

class Enemy : public Robot
{
public:
    Enemy();
    virtual RobotSetpoint getTargetSetpoint(int levelMode) = 0;

protected:
    Player *player;
    float time;
    MazePosition initialPosition;
    int dotsForFree;

    void resetTime();
    int getTimeState();

    std::array<bool, 4> freeTiles; // down, right, up, left

    void checkFreeTiles();

    int lock;
    void update(float deltaTime);
    void move();
    void setRobotMode(int levelMode);
    void enableFree(float time);

private:
    void moveEnemy();
    void findPath(RobotSetpoint targetSetpoint);
};

#endif