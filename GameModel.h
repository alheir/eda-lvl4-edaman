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

#include <array>

#define MAZE_WIDTH 28
#define MAZE_HEIGHT 36
#define MAZE_SIZE (MAZE_WIDTH * MAZE_HEIGHT)

#define NORMAL_MODE 1
#define BLINKING_MODE 2
#define RETURN_CAGE 3

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
    void newLevel(std::string maze);
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

    int eatenDots;

    int score; // agregado
    int lives;
    std::list<int> eatenFruits;
    std::array<bool, 4> eatenEnemies;
    int enemyScore;
    bool firstBloodFlag;

    int checkRobotCollision();
    void loseLife();
    void eatEnemy(int crashedRobot);

    int timeLord;
    bool waitingForTimeLord = false;
    void deployTimeLord(int seconds);
};

#endif
