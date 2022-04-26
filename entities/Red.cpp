/**
 * @file Red.cpp
 * @authors CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Clase de robot red
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Red.h"
#include <iostream>

using namespace std;

const MazePosition scatteringPoint = { 24, 0 };

Red::Red(MQTTClient *mqttClient, GameModel *gameModel, Player *player)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot2";
    this->player = player;

    imageIndex = 16;
    eyesColor = RED;    

    setRobotMode(NORMAL_MODE);
}

void Red::start()
{
    free = false;
    direction = 0;
    lock = 0;
    crash = false;

    imageIndex = 16;
    eyesColor = RED;
    dotsForFree = 0;
    timeForFree = 2;

    initialPosition = { 13, 14 };
    mazePosition = initialPosition;
    setPoint = getSetpoint(mazePosition, 0.0f);

    setRobotMode(NORMAL_MODE);
}

RobotSetpoint Red::getTargetSetpoint(int levelMode)
{
    RobotSetpoint returnSetpoint;

    if (levelMode == NORMAL_MODE)
    {
        switch (getTimeState())
        {
            case DISPERSION:
            {
                returnSetpoint = getSetpoint(scatteringPoint, setPoint.rotation);
                break;
            }
            case PERSECUTION:
            {
                returnSetpoint = player->getSetpoint();
                break;
            }
        }
    }

    else if (levelMode == BLINKING_MODE)
    {
        MazePosition targetTile = {GetRandomValue(0, MAZE_WIDTH), GetRandomValue(0, MAZE_HEIGHT)};
        returnSetpoint = getSetpoint(targetTile, setPoint.rotation);
    }

    else
    {
        returnSetpoint = setPoint;
    }

    return returnSetpoint;
}