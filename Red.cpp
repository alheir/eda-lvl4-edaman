#include "Red.h"
#include <iostream>

using namespace std;

const MazePosition scatteringPoint = {0, 24};

Red::Red(MQTTClient *mqttClient, GameModel *gameModel, Player *player)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot2";
    this->player = player;
    step = 0.1f / 12;
}

void Red::start()
{
    free = true;

    direction = UP;
    lock = 0;
    //mazePosition = {13, 14};
    mazePosition = { 26, 4 };  // para debug
    setPoint = getRobotSetpoint(mazePosition, 0.0f);
    //setPoint.positionX = +0.0025f;
    liftTo(setPoint.positionX, setPoint.positionZ);
    WaitTime(8000);

    setDisplay(16);
    // setDisplayColor(RED);
    setEyes(RED, RED);
}

void Red::update(float deltaTime)
{
    time += deltaTime;
    
    if (!lock)
    {
        mode = getTimeState();
        if (mode == DISPERSION)
        {
            findPath(getRobotSetpoint((26, 4), 0.0f));
        }
        else if (mode == PERSECUTION)
        {
            findPath(player->getSetpoint());
        }
    }

    moveEnemy();
}