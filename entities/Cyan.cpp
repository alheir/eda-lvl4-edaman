/**
 * @file Cyan.cpp
 * @authors CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Clase de robot cyan
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Cyan.h"

const MazePosition scatteringPoint = { 24, 35 };

Cyan::Cyan(MQTTClient *mqttClient, GameModel *gameModel, Player *player, Enemy *red)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot4";
    this->player = player;
    this->red = red;

    
    // setPoint.positionX = +0.0025f;

    imageIndex = 20;
    eyesColor = SKYBLUE;
    

    setRobotMode(NORMAL_MODE);

    /*liftTo(setPoint.positionX, setPoint.positionZ);
    WaitTime(7500);*/
}

void Cyan::start()
{
    free = false;
    //free = true; // para debug
    direction = 0;
    lock = 0;
    crash = false;

    imageIndex = 20;
    eyesColor = SKYBLUE;
    dotsForFree = 30;
    timeForFree = 6;
    //mustLeave = true;

    initialPosition = { 12, 17 };
    mazePosition = initialPosition;
    setPoint = getRobotSetpoint(mazePosition, 0.0f);

    setRobotMode(SETUP_MODE);

    //setPoint.positionX -= 0.1;

    /*liftTo(setPoint.positionX, setPoint.positionZ);
    WaitTime(7500);*/
}

RobotSetpoint Cyan::getTargetSetpoint(int levelMode)
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
                RobotSetpoint redPosition = red->getSetpoint();

                switch (player->getDirection())
                {
                    case UP:
                    {
                        newPosition.positionZ += 0.2f;
                        break;
                    }
                    case DOWN:
                    {
                        newPosition.positionZ -= 0.2f;
                        break;
                    }
                    case LEFT:
                    {
                        newPosition.positionX -= 0.2f;
                        break;
                    }
                    case RIGHT:
                    {
                        newPosition.positionX += 0.2f;
                        break;
                    }
                }
            
                newPosition.positionX += (newPosition.positionX - redPosition.positionX);
                newPosition.positionZ += (newPosition.positionZ - redPosition.positionZ);

                returnSetpoint = newPosition;
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