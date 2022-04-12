#include "Pink.h"

Pink::Pink(MQTTClient *mqttClient, GameModel *gameModel , Player *player)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot3";
    this->player = player;
}

void Pink::start()
{
    free = true;

    mazePosition = {13, 17};
    setPoint = getRobotSetpoint(mazePosition, 0.0f);
    setPoint.positionX = +0.0025f;
    liftTo(setPoint.positionX, setPoint.positionZ);

    setDisplay(18);
    // setDisplayColor(PINK);
    setEyes(PINK, PINK);
}

void Pink::update(float deltaTime)
{
    //setSetpoint(setPoint);
}