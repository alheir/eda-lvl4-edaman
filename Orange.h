// Clase de orange

#ifndef ORANGE_H
#define ORANGE_H

#include "Enemy.h"

class Orange : public Enemy
{
public:
    Orange(MQTTClient* mqttClient, GameModel* gameModel, Player* player);

    void start();
    RobotSetpoint getTargetSetpoint(int levelMode);

private:
    
};

#endif