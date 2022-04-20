// Clase de pink

#ifndef PINK_H
#define PINK_H

#include "Enemy.h"

class Pink : public Enemy
{
public:
    Pink(MQTTClient *mqttClient, GameModel *gameModel, Player *player);

    void start();

    RobotSetpoint getTargetSetpoint(int levelMode);

private:
    
};

#endif