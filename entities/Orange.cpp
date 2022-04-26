/**
 * @file Orange.cpp
 * @authors CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Orange robot class
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Orange.h"

const MazePosition scatteringPoint = {3, 35};

/**
 * @brief Construct a new Orange object
 *
 * @param mqttClient
 * @param gameModel
 * @param player Address of player robot
 */
Orange::Orange(MQTTClient *mqttClient, GameModel *gameModel, Player *player)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot5";
    this->player = player;

    imageIndex = 22;
    eyesColor = ORANGE;

    setRobotMode(NORMAL_MODE);
}

/**
 * @brief Initializes the robot
 *
 */
void Orange::start()
{
    free = false;
    direction = 0;
    lock = 0;
    crash = false;

    imageIndex = 22;
    eyesColor = ORANGE;
    dotsForFree = 60;
    timeForFree = 7;

    initialPosition = {16, 18};
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
RobotSetpoint Orange::getTargetSetpoint(int levelMode)
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
            Vector2 vector = {setPoint.positionX - player->getSetpoint().positionX,
                              setPoint.positionZ - player->getSetpoint().positionZ};

            if ((vector.x * vector.x) + (vector.y * vector.y) > 0.64f)
            {
                returnSetpoint = player->getSetpoint();
            }
            else
            {
                returnSetpoint = getSetpoint(scatteringPoint, setPoint.rotation);
            }
            break;
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