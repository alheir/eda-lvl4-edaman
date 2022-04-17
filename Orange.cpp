#include "Orange.h"
#include <cmath>

const MazePosition scatteringPoint = {35, 0};

Orange::Orange(MQTTClient *mqttClient, GameModel *gameModel , Player *player)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot5";
    this->player = player;
    step = 0.1f / 12;
}

void Orange::start()
{
    free = false;
    free = true;    // para debug

    direction = UP;
    lock = 0;
    //mazePosition = {15, 17};
    mazePosition = { 1, 32 };  // para debug
    setPoint = getRobotSetpoint(mazePosition, 0.0f);
    //setPoint.positionX = +0.0025f;
    liftTo(setPoint.positionX, setPoint.positionZ);
    WaitTime(8000);

    setDisplay(22);
    // setDisplayColor(ORANGE);
    setEyes(ORANGE, ORANGE);
}

void Orange::update(float deltaTime)
{
    time += deltaTime;
    
    if (!lock)
    {
        mode = getTimeState();
        if (mode == DISPERSION)
        {
            findPath(getRobotSetpoint((1, 32), 0.0f));
        }
        else if (mode == PERSECUTION)
        {

            Vector2 vector = { setPoint.positionX - player->getSetpoint().positionX, setPoint.positionZ - player->getSetpoint().positionZ };
            if ((vector.x * vector.x) + (vector.y * vector.y) < 6400)
            {
                findPath(player->getSetpoint());
            }
            else
            {
                findPath(getRobotSetpoint(scatteringPoint, 0.0f));
            }
        }
    }

    moveEnemy();
}