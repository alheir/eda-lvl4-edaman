/**
 * EDA-Man game model
 *
 * Copyright (C) 2022 Marc S. Ressl
 *
 * Controls the game model.
 */

#ifndef _GAMEMODEL_H
#define _GAMEMODEL_H

class Robot;

struct MazePosition
{
    MazePosition(int x = 0, int y = 0)
    {
        this->x = x;
        this->y = y;
    }

    int x;
    int y;
};

enum GameState
{
    GameStart,
    GameStarting,
    GamePlaying
};

#include <string>
#include <list>
#include <vector>

#include <raylib.h>

#include "MQTTClient.h"

#include "GameView.h"
#include "Robot.h"

class GameModel
{
public:
    GameModel(MQTTClient* mqttClient);

    void setGameView(GameView* gameView);

    void start(std::string maze);
    
    void update(float deltaTime);

    void addRobot(Robot* robot);

    bool isTileFree(const MazePosition &position);

    // nuestros metodos
    bool shouldEndLevel();
    void newLevel(std::string maze);
    void pickItem(MazePosition* position);

private:
    MQTTClient *mqttClient;
    GameView *gameView;

    std::string maze;
    std::vector<Robot *> robots;

    int gameState;
    float gameStateTime;

    int remainingDots;
    int remainingEnergizers;

    int score;  // agregado
    int lives;
    std::list<int> eatenFruits;
};

#endif
