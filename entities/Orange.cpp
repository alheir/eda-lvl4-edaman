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

const MazePosition scatteringPoint = { 3, 35 };

Orange::Orange(MQTTClient *mqttClient, GameModel *gameModel, Player *player)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot5";
    this->player = player;

    
    // setPoint.positionX = +0.0025f;

    imageIndex = 22;
    eyesColor = ORANGE;
    

    setRobotMode(NORMAL_MODE);

    /*liftTo(setPoint.positionX, setPoint.positionZ);
    WaitTime(7500);*/
}

void Orange::start()
{
    free = false;
    //free = true; // para debug
    direction = 0;
    lock = 0;
    crash = false;

    imageIndex = 22;
    eyesColor = ORANGE;
    dotsForFree = 60;
    timeForFree = 8;
    mustLeave = true;

    initialPosition = { 16, 18 }; //{ 16, 17 }
    mazePosition = initialPosition;
    setPoint = getRobotSetpoint(mazePosition, 0.0f);

    setRobotMode(NORMAL_MODE);

    setPoint.positionX += 0.05;

    liftTo(setPoint.positionX, setPoint.positionZ);
    WaitTime(7500);
}

RobotSetpoint Orange::getTargetSetpoint(int levelMode)
{
    RobotSetpoint returnSetpoint;

    if (levelMode == NORMAL_MODE)
    {
        switch (getTimeState())
        {
            case DISPERSION:
            {
                returnSetpoint = getRobotSetpoint(scatteringPoint, setPoint.rotation);
                break;
            }
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