/**
 * @file Cyan.cpp
 * @authors CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Cyan robot class
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Cyan.h"

const MazePosition scatteringPoint = {24, 35};

/**
 * @brief Construct a new Cyan object
 *
 * @param mqttClient
 * @param gameModel
 * @param player Address of player robot
 * @param red Address of red enemy robot
 */
Cyan::Cyan(MQTTClient *mqttClient, GameModel *gameModel, Player *player, Enemy *red)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot4";
    this->player = player;
    this->red = red;

    imageIndex = 20;
    eyesColor = SKYBLUE;

    setRobotMode(NORMAL_MODE);
}

/**
 * @brief Initializes the robot
 *
 */
void Cyan::start()
{
    free = false;
    direction = 0;
    lock = 0;
    crash = false;

    imageIndex = 20;
    eyesColor = SKYBLUE;
    dotsForFree = 30;
    timeForFree = 5;

    initialPosition = {11, 18};
    mazePosition = initialPosition;
    setPoint = getSetpoint(mazePosition, 0.0f);

    setRobotMode(NORMAL_MODE);
}

/**
 * @brief Returns the robot target setpoint based on current level mode
 *
 * @param levelMode
 * @return RobotSetpoint
 */
RobotSetpoint Cyan::getTargetSetpoint(int levelMode)
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
        MazePosition targetTile = {GetRandomValue(0, MAZE_WIDTH),
                                   GetRandomValue(0, MAZE_HEIGHT)};
        returnSetpoint = getSetpoint(targetTile, setPoint.rotation);
    }

    else
    {
        returnSetpoint = setPoint;
    }

    return returnSetpoint;
}