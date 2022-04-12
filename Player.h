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

    void setDirection(int xDir, int yDir);

    RobotSetpoint getSetpoint();
    MazePosition getMazePosition();

private:
    bool moving;
    float step;
    char direction;
    MazePosition nextTile;

    bool shouldMove();
};

#endif