/**
 * @file Cyan.h
 * @authors CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Clase de robot cyan
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef CYAN_H
#define CYAN_H

#include "Enemy.h"

class Cyan : public Enemy
{
public:
    Cyan(MQTTClient *mqttClient, GameModel *gameModel, Player *player, Enemy *red);
    void start();
    RobotSetpoint getTargetSetpoint(int levelMode);

private:
    Enemy *red;
};

#endif