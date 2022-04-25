/**
 * @file Player.cpp
 * @authors CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Clase de robot player
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Player.h"
#include <iostream>

using namespace std;

const MazePosition initialPoint = {13, 26};

Player::Player(MQTTClient *mqttClient, GameModel *gameModel)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot1";

    eyesColor = YELLOW;
}

void Player::start()
{
    crash = false;

    mazePosition = {13, 26};
    setPoint = getRobotSetpoint(mazePosition, 0.0f);
    //setPoint.positionX = +0.0025f;
    direction = UP;
    liftTo(setPoint.positionX, setPoint.positionZ);

    setRobotMode(false);

    forceMove();
}

void Player::update(float deltaTime)
{
    gameModel->pickItem(&mazePosition);

    if (crash)
    {
        if (gameModel->getLevelMode() == NORMAL_MODE)
        {
            cout << "AAAAAAAAAAAAAAAA ME MUEEEEEEEERO :C" << endl;
        }
        else if (gameModel->getLevelMode() == BLINKING_MODE)
        {
            cout << "TE COMI FANTASMA QLIA" << endl;
        }
    }

    if (shouldMove())
    {
        RobotSetpoint tempSetPoint = getRobotSetpoint(nextTile, setPoint.rotation);
        static bool shouldUpdateTile = false;

        switch (direction)
        {
        case UP:
        {
            setPoint.rotation = 0.0f;

            if (setPoint.positionZ < tempSetPoint.positionZ)
                setPoint.positionZ += step;

            shouldUpdateTile = (tempSetPoint.positionZ - setPoint.positionZ) < 0.0025f;

            break;
        }
        case DOWN:
        {
            setPoint.rotation = 180.0f;

            if (setPoint.positionZ > tempSetPoint.positionZ)
                setPoint.positionZ -= step;

            shouldUpdateTile = (setPoint.positionZ - tempSetPoint.positionZ) < 0.0025f;

            break;
        }
        case LEFT:
        {
            setPoint.rotation = 270.0f;

            if (setPoint.positionX > tempSetPoint.positionX)
                setPoint.positionX -= step;

            shouldUpdateTile = (setPoint.positionX - tempSetPoint.positionX) < 0.00025f;

            break;
        }
        case RIGHT:
        {
            setPoint.rotation = 90.0f;

            if (setPoint.positionX < tempSetPoint.positionX)
                setPoint.positionX += step;

            shouldUpdateTile = (tempSetPoint.positionX - setPoint.positionX) < 0.00025f;

            break;
        }
        }

        if (shouldUpdateTile)
        {
            mazePosition = nextTile;
            shouldUpdateTile = false;
            setRobotMode(true);
        }
    }
    else
        setRobotMode(false);

    setSetpoint(setPoint);
}

void Player::setRobotMode(bool isMoving)
{
    if (isMoving)
    {
        if (imageIndex == 1)
            imageIndex = 2;
        else
            imageIndex = 1;

        setDisplay(imageIndex);
        setEyes(eyesColor, eyesColor);
    }
    else
    {
        imageIndex = 0;
        setDisplay(imageIndex);
        setEyes(eyesColor, eyesColor);
    }

    if (gameModel->getLevelMode() == NORMAL_MODE)
        step = 0.64f / 60.0f;
    else
        step = 0.72f / 60.0f;
}

void Player::setDirection(int xDir, int yDir)
{
    char lastDirection = direction;

    if (xDir || yDir)
    {
        if (xDir > 0)
            direction = RIGHT;
        else if (xDir < 0)
            direction = LEFT;
        else if (yDir > 0)
            direction = UP;
        else if (yDir < 0)
            direction = DOWN;

        if (!shouldMove())
            direction = lastDirection;
    }
}

bool Player::shouldMove()
{
    nextTile = mazePosition;

    switch (direction)
    {
    case UP:

        nextTile.y--;
        break;

    case DOWN:

        nextTile.y++;
        break;

    case LEFT:

        nextTile.x--;
        break;

    case RIGHT:

        nextTile.x++;
        break;

    default:
        break;
    }

    return (gameModel->isTileFree(nextTile));
}
