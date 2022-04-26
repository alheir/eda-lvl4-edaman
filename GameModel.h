/**
 * @file GameModel.h
 * @authors CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Control del game model
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _GAMEMODEL_H
#define _GAMEMODEL_H

#define MAZE_WIDTH 28
#define MAZE_HEIGHT 36
#define MAZE_SIZE (MAZE_WIDTH * MAZE_HEIGHT)

enum LevelMode {
    SETUP_MODE = 1,
    NORMAL_MODE,
    BLINKING_MODE,
    RETURN_CAGE
};

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
    GamePlaying,
    GameFinish
};

#include <string>
#include <list>
#include <vector>

#include <raylib.h>

#include "MQTTClient.h"

#include "GameView.h"
#include "entities/Robot.h"

class GameModel
{
public:
    GameModel(MQTTClient *mqttClient);

    void setGameView(GameView *gameView);
    void start(std::string maze);
    void update(float deltaTime);
    void addRobot(Robot *robot);
    bool isTileFree(const MazePosition &position);

    // nuestros metodos
    bool shouldEndLevel();
    void nextScreen(std::string maze);
    void pickItem(MazePosition *position);
    int getLevelMode();

private:
    MQTTClient *mqttClient;
    GameView *gameView;

    std::string maze;
    std::vector<Robot *> robots;

    int gameState;
    float gameStateTime;
    int levelMode;

    int remainingDots;
    int remainingEnergizers;

    int score; // agregado
    int lives;
    std::list<int> eatenFruits;
    bool eatenEnemies[4];
    int enemyScore;
    int eatenDots;

    int checkRobotCollision();
    void loseLife();
    void eatEnemy(int crashedRobot);
    void enableFree();
};

#endif
