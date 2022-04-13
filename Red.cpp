#include "Red.h"

const MazePosition scatteringPoint = {0, 24};

Red::Red(MQTTClient *mqttClient, GameModel *gameModel, Player *player)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot2";
    this->player = player;
    step = 0.008f;
}

void Red::start()
{
    free = true;

    mazePosition = {13, 14};
    setPoint = getRobotSetpoint(mazePosition, 0.0f);
    setPoint.positionX = +0.0025f;
    //liftTo(setPoint.positionX, setPoint.positionZ);

    setDisplay(16);
    // setDisplayColor(RED);
    setEyes(RED, RED);
}

void Red::update(float deltaTime)
{
    int direction = findPath(player->getSetpoint());

    switch (direction)
    {
    case UP:
        setPoint.positionZ += step;
        break;

    case DOWN:
        setPoint.positionZ -= step;

        break;

    case LEFT:
        setPoint.positionX -= step;

        break;

    case RIGHT:
        setPoint.positionX += step;

        break;

    default:
        break;
    }

    setSetpoint(setPoint);
}