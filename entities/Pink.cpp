/**
 * @file Pink.cpp
 * @authors CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Clase de robot pink
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Pink.h"

const MazePosition scatteringPoint = { 3, 0 };

Pink::Pink(MQTTClient *mqttClient, GameModel *gameModel, Player *player)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot3";
    this->player = player;

    
    // setPoint.positionX = +0.0025f;

    imageIndex = 18;
    eyesColor = PINK;
    

    setRobotMode(NORMAL_MODE);

    /*liftTo(setPoint.positionX, setPoint.positionZ);
    WaitTime(7500);*/
}

void Pink::start()
{
    free = false;
    direction = 0;
    lock = 0;
    crash = false;

    imageIndex = 18;
    eyesColor = PINK;
    dotsForFree = 0;
    timeForFree = 4;
    //mustLeave = true;

    initialPosition = { 13, 17 };
    mazePosition = initialPosition;
    setPoint = getRobotSetpoint(mazePosition, 0.0f);

    setRobotMode(SETUP_MODE);

    //setPoint.positionX += 0;

    /*liftTo(setPoint.positionX, setPoint.positionZ);
    WaitTime(7500);*/
}

RobotSetpoint Pink::getTargetSetpoint(int levelMode)
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
                RobotSetpoint newPosition = player->getSetpoint();

                switch (player->getDirection())
                {
                    case UP:
                    {
                        newPosition.positionZ += 0.4f;
                        break;
                    }
                    case DOWN:
                    {
                        newPosition.positionZ -= 0.4f;
                        break;
                    }
                    case LEFT:
                    {
                        newPosition.positionX -= 0.4f;
                        break;
                    }
                    case RIGHT:
                    {
                        newPosition.positionX += 0.4f;
                        break;
                    }
                }
                returnSetpoint = newPosition;
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