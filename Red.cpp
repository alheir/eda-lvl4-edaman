#include "Red.h"

Red::Red(MQTTClient *mqttClient, GameModel *gameModel, Player *player)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot2";
    this->player = player;
}

void Red::start()
{
    free = true;

    mazePosition = {13, 14};
    setPoint = getRobotSetpoint(mazePosition, 0.0f);
    setPoint.positionX = +0.0025f;
    liftTo(setPoint.positionX, setPoint.positionZ);

    setDisplay(16);
    // setDisplayColor(RED);
    setEyes(RED, RED);
}

void Red::update(float deltaTime)
{
    //setSetpoint(setPoint);
}