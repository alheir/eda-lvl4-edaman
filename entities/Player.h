// Clase del jugador (pacman)

#ifndef PLAYER_H
#define PLAYER_H

#include "Robot.h"

class Player : public Robot
{
public:
    Player(MQTTClient *mqttClient, GameModel *gameModel);

    void start();
    void update(float deltaTime);
    void move();

    void setDirection(int xDir, int yDir);

private:
    //bool moving;
    MazePosition nextTile;

    bool shouldMove();
    void setRobotMode(bool isMoving);
};

#endif