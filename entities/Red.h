/**
 * @file Red.h
 * @authors CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Red robot class
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef RED_H
#define RED_H

#include "Enemy.h"

class Red : public Enemy
{
public:
    Red(MQTTClient *mqttClient, GameModel *gameModel, Player *player);
    void start();

    RobotSetpoint getTargetSetpoint(int levelMode);

private:
};

#endif