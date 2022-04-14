#include "Orange.h"
#include <cmath>

const MazePosition scatteringPoint = {35, 0};

Orange::Orange(MQTTClient *mqttClient, GameModel *gameModel , Player *player)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot5";
    this->player = player;

}

void Orange::start()
{
    free = false;

    mazePosition = {15, 17};
    setPoint = getRobotSetpoint(mazePosition, 0.0f);
    setPoint.positionX = +0.0025f;
    liftTo(setPoint.positionX, setPoint.positionZ);

    setDisplay(22);
    // setDisplayColor(ORANGE);
    setEyes(ORANGE, ORANGE);
}

void Orange::update(float deltaTime)
{
    Vector2 vector = {setPoint.positionX - player->getSetpoint().positionX, setPoint.positionZ - player->getSetpoint().positionZ};
    if ((vector.x * vector.x) + (vector.y * vector.y) < 6400)  
    {
        findPath(player->getSetpoint());
    }
    else
    {
        findPath(getRobotSetpoint(scatteringPoint, 0.0f));
    }
}