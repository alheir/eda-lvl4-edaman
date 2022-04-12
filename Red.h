// Clase de red

#ifndef RED_H
#define RED_H

#include "Enemy.h"

class Red : public Enemy
{
public:
    Red(MQTTClient *mqttClient, GameModel *gameModel, Player *player);

    void start();
    void update(float deltaTime);

private:
};

#endif