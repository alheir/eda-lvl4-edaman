/**
 * @file GameModel.h
 * @authors RESSL ~ CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Controls EDA-Man behavior
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

enum LevelMode
{
    NORMAL_MODE = 1,
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

#include <raylib.h>
#include <array>

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

    bool shouldEndLevel();
    bool shouldEndGame();
    void nextScreen(std::string maze);
    void pickItem(const MazePosition &position);
    int getLevelMode();

private:
    MQTTClient *mqttClient;
    GameView *gameView;

    std::string maze;
    std::string mazeBack;
    std::vector<Robot *> robots;

    int gameState;
    int levelMode;
    int remainingDots;
    int remainingEnergizers;
    float gameStateTime;
    int lives;
    int score;
    int highscore;
    int run;
    int enemyScore;
    int enemyScoreIndex;

    std::list<int> eatenFruits;
    int currentActiveFruitIndex;
    MazePosition currentActiveFruitPosition;
    float fruitTimer;
    bool fruitActive;

    std::array<bool, 4> eatenEnemies;
    int eatenDots;

    bool playingEatingDotSound;
    bool playingEndGameSound;

    int checkRobotCollision();
    void loseLife();
    void eatEnemy(int crashedRobot);
    void enableFree();

    MazePosition getRandomFreePosition();
};

#endif
