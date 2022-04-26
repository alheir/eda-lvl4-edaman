/**
 * @file Player.h
 * @authors CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Player robot class
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "Robot.h"

class Player : public Robot
{
public:
    Player(MQTTClient *mqttClient, GameModel *gameModel);

    void start();
    void update(float deltaTime);
    void move();

    void setDirection(int xDir, int yDir);

private:
    MazePosition nextTile;

    bool shouldMove();
    void setRobotMode(int isMoving);
};

#endif