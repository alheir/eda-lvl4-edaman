/**
 * @file Orange.cpp
 * @authors CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Clase de robot orange
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Orange.h"

const MazePosition scatteringPoint = {3, 35};

Orange::Orange(MQTTClient *mqttClient, GameModel *gameModel, Player *player)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot5";
    this->player = player;

    imageIndex = 22;
}

void Orange::start()
{
    mazePosition = {16, 17};
    // mazePosition = {1, 32}; // para debug
    setPoint = getRobotSetpoint(mazePosition, 270.0f);
    direction = LEFT;

    imageIndex = 22;
    eyesColor = ORANGE;

    setRobotMode(NORMAL_MODE);

    free = false;
    lock = 0;
    crash = false;

    forceMove();
}

RobotSetpoint Orange::getTargetSetpoint(int levelMode)
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
        {
            Vector2 vector = {setPoint.positionX - player->getSetpoint().positionX,
                              setPoint.positionZ - player->getSetpoint().positionZ};

            if ((vector.x * vector.x) + (vector.y * vector.y) > 0.64f)
            {
                returnSetpoint = player->getSetpoint();
            }
            else
            {
                returnSetpoint = getRobotSetpoint(scatteringPoint, setPoint.rotation);
            }

            break;
        }

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