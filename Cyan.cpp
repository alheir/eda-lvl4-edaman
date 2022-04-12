#include "Cyan.h"

Cyan::Cyan(MQTTClient *mqttClient, GameModel *gameModel , Player *player)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot4";
    this->player = player;
}

void Cyan::start()
{
    free = false;

    mazePosition = {11, 17};
    setPoint = getRobotSetpoint(mazePosition, 0.0f);
    setPoint.positionX = +0.0025f;
    liftTo(setPoint.positionX, setPoint.positionZ);

    setDisplay(20);
    // setDisplayColor(SKYBLUE);
    setEyes(SKYBLUE, SKYBLUE);
}

void Cyan::update(float deltaTime)
{
    //setSetpoint(setPoint);
}