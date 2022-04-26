/**
 * @file Orange.h
 * @authors CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Orange robot class
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef ORANGE_H
#define ORANGE_H

#include "Enemy.h"

class Orange : public Enemy
{
public:
    Orange(MQTTClient *mqttClient, GameModel *gameModel, Player *player);

    void start();
    RobotSetpoint getTargetSetpoint(int levelMode);

private:
};

#endif