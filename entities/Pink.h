// Clase de pink

/**
 * @file Pink.h
 * @authors CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Clase de robot pink
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */

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