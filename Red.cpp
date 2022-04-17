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

    setDisplay(16);
    // setDisplayColor(RED);
    setEyes(RED, RED);

    //mazePosition = {13, 14};
    mazePosition = { 26, 4 };  // para debug
    setPoint = getRobotSetpoint(mazePosition, 0.0f);
    //setPoint.positionX = +0.0025f;
    //liftTo(setPoint.positionX, setPoint.positionZ);
    //WaitTime(7000);
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

    setDisplay(16);
    // setDisplayColor(RED);
    setEyes(RED, RED);
}

RobotSetpoint Red::getTargetSetpoint(int levelMode)
{
    if (levelMode == NORMAL_MODE)
    {
        switch (getTimeState())
        {
            case DISPERSION:
            {
                return getRobotSetpoint(scatteringPoint, setPoint.rotation);
            }
            case PERSECUTION:
            {
                return player->getSetpoint();
            }
        }
    }
    else if (levelMode == BLINKING_MODE)
    {
        MazePosition targetTile = { GetRandomValue(0, MAZE_WIDTH), GetRandomValue(0, MAZE_HEIGHT) };
        return getRobotSetpoint(targetTile, setPoint.rotation);
    }
    else
        return setPoint;
}