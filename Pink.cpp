#include "Pink.h"

const MazePosition scatteringPoint = {0, 3}; 

Pink::Pink(MQTTClient *mqttClient, GameModel *gameModel , Player *player)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot3";
    this->player = player;
    step = 0.1f / 12;
}

void Pink::start()
{
    free = true;

    direction = UP;
    lock = 0;
    //mazePosition = {13, 17};
    mazePosition = { 26, 4 };  // para debug
    setPoint = getRobotSetpoint(mazePosition, 0.0f);
    //setPoint.positionX = +0.0025f;
    liftTo(setPoint.positionX, setPoint.positionZ);
    WaitTime(8000);

    setDisplay(18);
    // setDisplayColor(PINK);
    setEyes(PINK, PINK);
}

void Pink::update(float deltaTime)
{
    if (!lock)
    {
        RobotSetpoint newPosition = player->getSetpoint();
        switch (player->getDirection())
        {
            case UP:
            {
                newPosition.positionZ + 0.4f;
                break;
            }
            case DOWN:
            {
                newPosition.positionZ - 0.4f;
                break;
            }
            case LEFT:
            {
                newPosition.positionX - 0.4f;
                break;
            }
            case RIGHT:
            {
                newPosition.positionX + 0.4f;
                break;
            }
        }

        findPath(newPosition);
    }
      
    moveEnemy();
}