#include "Orange.h"
#include <cmath>

const MazePosition scatteringPoint = {0,35};

Orange::Orange(MQTTClient *mqttClient, GameModel *gameModel , Player *player)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot5";
    this->player = player;

    //mazePosition = {15, 17};
    mazePosition = { 1, 32 };  // para debug
    setPoint = getRobotSetpoint(mazePosition, 0.0f);
    //setPoint.positionX = +0.0025f;

    imageIndex = 22;
    eyesColor = ORANGE;

    setRobotMode(NORMAL_MODE);

    //liftTo(setPoint.positionX, setPoint.positionZ);
    //WaitTime(7000);
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

    imageIndex = 22;
    eyesColor = ORANGE;

    setRobotMode(NORMAL_MODE);
}

RobotSetpoint Orange::getTargetSetpoint(int levelMode)
{
    if (levelMode == NORMAL_MODE)
    {
        switch (getTimeState())
        {
            case DISPERSION:
            {
                return getRobotSetpoint((1, 32), setPoint.rotation);
            }
            case PERSECUTION:
            {
                Vector2 vector = { setPoint.positionX - player->getSetpoint().positionX, setPoint.positionZ - player->getSetpoint().positionZ };
                if ((vector.x * vector.x) + (vector.y * vector.y) < 6400)
                {
                    return player->getSetpoint();
                }
                else
                {
                    getRobotSetpoint(scatteringPoint, setPoint.rotation);
                }
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