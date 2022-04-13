#include "Player.h"
#include <cmath>

Player::Player(MQTTClient *mqttClient, GameModel *gameModel)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot1";

    moving = false;
    step = 0.01f;
    direction = UP;
}

void Player::start()
{
    mazePosition = {13, 26};
    setPoint = getRobotSetpoint(mazePosition, 0.0f);
    setPoint.positionX = +0.0025f;
    liftTo(setPoint.positionX, setPoint.positionZ);

    setDisplay(1);
    // setDisplayColor(YELLOW);
    setEyes(YELLOW, YELLOW);
}

void Player::update(float deltaTime)
{
    if (shouldMove())
    {
        RobotSetpoint tempSetPoint = getRobotSetpoint(nextTile, setPoint.rotation);
        static bool shouldUpdateTile = false;

        switch (direction)
        {
        case UP:
            if (setPoint.positionZ < tempSetPoint.positionZ)
                setPoint.positionZ += step;

            shouldUpdateTile = (tempSetPoint.positionZ - setPoint.positionZ) < 0.0025f;

            break;

        case DOWN:
            if (setPoint.positionZ > tempSetPoint.positionZ)
                setPoint.positionZ -= step;

            shouldUpdateTile = (setPoint.positionZ - tempSetPoint.positionZ) < 0.0025f;

            break;

        case LEFT:
            if (setPoint.positionX > tempSetPoint.positionX)
                setPoint.positionX -= step;

            shouldUpdateTile = (setPoint.positionX - tempSetPoint.positionX) < 0.00025f;

            break;

        case RIGHT:
            if (setPoint.positionX < tempSetPoint.positionX)
                setPoint.positionX += step;

            shouldUpdateTile = (tempSetPoint.positionX - setPoint.positionX) < 0.00025f;

            break;

        default:
            break;
        }

        if (shouldUpdateTile)
        {
            mazePosition = nextTile;
            shouldUpdateTile = false;
        }
    }

    gameModel->pickItem(&mazePosition);
    setSetpoint(setPoint);
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

        if (shouldMove())
            setPoint.rotation = 180.0f * (float)atan2(-yDir, xDir) / (float)PI + 90.0f;

        else
            direction = lastDirection;
    }
}

int Player::getDirection()
{
    return direction;
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
