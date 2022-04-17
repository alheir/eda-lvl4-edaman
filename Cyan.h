// Clase de cyan

#ifndef CYAN_H
#define CYAN_H

#include "Enemy.h"

class Cyan : public Enemy
{
public:
    Cyan(MQTTClient* mqttClient, GameModel* gameModel, Player* player, Enemy* red);

    void start();

private:
    RobotSetpoint getTargetSetpoint(int levelMode);
    Enemy* red;
};

#endif