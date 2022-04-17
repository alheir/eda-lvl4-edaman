#include "Cyan.h"

const MazePosition scatteringPoint = {35, 24};

Cyan::Cyan(MQTTClient *mqttClient, GameModel *gameModel, Player *player, Enemy *red)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    this->robotId = "robot4";
    this->player = player;
    this->red = red;
    step = 0.1f / 12;
}

void Cyan::start()
{
    free = false;
    free = true;    // para debug

    direction = UP;
    lock = 0;
    //mazePosition = {11, 17};
    mazePosition = { 26, 32 };  // para debug
    setPoint = getRobotSetpoint(mazePosition, 0.0f);
    //setPoint.positionX = +0.0025f;
    liftTo(setPoint.positionX, setPoint.positionZ);
    WaitTime(8000);

    setDisplay(20);
    // setDisplayColor(SKYBLUE);
    setEyes(SKYBLUE, SKYBLUE);
}

void Cyan::update(float deltaTime)
{
    time += deltaTime;
    
    if (!lock)
    {
        mode = getTimeState();
        if (mode == DISPERSION)
        {
            findPath(getRobotSetpoint((26, 32), 0.0f));
        }
        else if (mode == PERSECUTION)
        {

            RobotSetpoint newPosition = player->getSetpoint();
            RobotSetpoint redPosition = red->getSetpoint();

            switch (player->getDirection())
            {
                case UP:
                {
                    newPosition.positionZ + 0.2f;
                    break;
                }
                case DOWN:
                {
                    newPosition.positionZ - 0.2f;
                    break;
                }
                case LEFT:
                {
                    newPosition.positionX - 0.2f;
                    break;
                }
                case RIGHT:
                {
                    newPosition.positionX + 0.2f;
                    break;
                }
            }

            newPosition.positionX += (newPosition.positionX - redPosition.positionX);
            newPosition.positionZ += (newPosition.positionZ - redPosition.positionZ);

            findPath(newPosition);
        }
    }

    moveEnemy();
}