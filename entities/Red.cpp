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

const MazePosition scatteringPoint = {24, 0};

Red::Red(MQTTClient *mqttClient, GameModel *gameModel, Player *player)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot2";
    this->player = player;

    imageIndex = 16;
    eyesColor = RED;
}

void Red::start()
{
    free = true;
    direction = 0;
    lock = 0;
    crash = false;

    mazePosition = {13, 14};
    // mazePosition = {26, 4}; // para debug
    setPoint = getRobotSetpoint(mazePosition, 270.0f);
    direction = LEFT;
    // setPoint.positionX = +0.0025f;

    imageIndex = 16;
    eyesColor = RED;

    setRobotMode(NORMAL_MODE);

    forceMove();
}

RobotSetpoint Red::getTargetSetpoint(int levelMode)
{
    RobotSetpoint returnSetpoint;

    if (levelMode == NORMAL_MODE)
    {
        switch (getTimeState())
        {
        case DISPERSION:
            returnSetpoint = getRobotSetpoint(scatteringPoint, setPoint.rotation);
            break;

        case PERSECUTION:
            returnSetpoint = player->getSetpoint();
            break;

        default:
            break;
        }
    }

    else if (levelMode == BLINKING_MODE)
    {
        MazePosition targetTile = {GetRandomValue(0, MAZE_WIDTH), GetRandomValue(0, MAZE_HEIGHT)};
        returnSetpoint = getRobotSetpoint(targetTile, setPoint.rotation);
    }

    else
    {
        returnSetpoint = setPoint;
    }

    return returnSetpoint;
}