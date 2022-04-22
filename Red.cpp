#include "Red.h"
#include <iostream>

using namespace std;

const MazePosition scatteringPoint = {24,0};

Red::Red(MQTTClient *mqttClient, GameModel *gameModel, Player *player)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot2";
    this->player = player;
    
    //mazePosition = {13, 14};
    mazePosition = { 26, 4 };  // para debug
    setPoint = getRobotSetpoint(mazePosition, 0.0f);
    //setPoint.positionX = +0.0025f;

    imageIndex = 16;
    eyesColor = RED;

    setRobotMode(NORMAL_MODE);

    //liftTo(setPoint.positionX, setPoint.positionZ);
    //WaitTime(7000);
}

void Red::start()
{
    free = true;
    direction = 0;
    lock = 0;
    crash = false;

    //mazePosition = {13, 14};
    mazePosition = { 26, 4 };  // para debug
    setPoint = getRobotSetpoint(mazePosition, 0.0f);
    //setPoint.positionX = +0.0025f;

    imageIndex = 16;
    eyesColor = RED;

    setRobotMode(NORMAL_MODE);
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
    else if (levelMode == RETURN_CAGE)
    {
        return getRobotSetpoint(scatteringPoint, 0.0f);
    }
    else
    {
        return setPoint;
    }
}