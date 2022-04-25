/**
 * @file GameModel.cpp
 * @authors CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Control del game model
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <iostream>
#include <vector>
#include <string>

#include "GameModel.h"

using namespace std;

const int SCORE_PER_DOT = 10;
const int SCORE_PER_ENERGIZER = 50;

GameModel::GameModel(MQTTClient *mqttClient)
{
    this->mqttClient = mqttClient;
}

void GameModel::setGameView(GameView *gameView)
{
    this->gameView = gameView;
}

void GameModel::addRobot(Robot *robot)
{
    robots.push_back(robot);
}

bool GameModel::isTileFree(const MazePosition &position)
{
    if ((position.x < 0) || (position.x >= MAZE_WIDTH))
        return false;

    if ((position.y < 0) || (position.y >= MAZE_HEIGHT))
        return false;

    char tile = maze[position.y * MAZE_WIDTH + position.x];

    return (tile == ' ') || (tile == '+') || (tile == '#');
}

int GameModel::checkRobotCollision()
{
    int crashedRobot = 0;

    for (int i = 0; i < robots.size(); i++)
    {
        MazePosition mazePosition = robots[i]->getMazePosition();
        MazePosition mazePosition1 = mazePosition;
        MazePosition mazePosition2 = mazePosition;

        int direction = robots[i]->getDirection();
        switch (direction)
        {
        case UP:
        {
            mazePosition1.y -= 1;
            mazePosition2.y -= 2;
            break;
        }
        case LEFT:
        {
            mazePosition1.x -= 1;
            mazePosition2.x -= 2;
            break;
        }
        case DOWN:
        {
            mazePosition1.y += 1;
            mazePosition2.y += 2;
            break;
        }
        case RIGHT:
        {
            mazePosition1.x += 1;
            mazePosition2.x += 2;
            break;
        }
        default:
        {
            cout << "Error in robot " << i << " direction" << endl;
            break;
        }
        }

        for (int j = 0; j < robots.size(); j++)
        {
            if (i != j)
            {
                MazePosition targetPosition = robots[j]->getMazePosition();

                if (i == 0 || j == 0)
                {
                    // detect collision between pacman and enemy
                    if (((targetPosition.x == mazePosition1.x) && (targetPosition.y == mazePosition1.y)))
                    {
                        robots[0]->crash = true;
                        crashedRobot = max(i, j);
                    }
                }
                else
                {
                    // avoid collisions between enemies
                    if (((targetPosition.x == mazePosition1.x) && (targetPosition.y == mazePosition1.y)) ||
                        ((targetPosition.x == mazePosition2.x) && (targetPosition.y == mazePosition2.y)))
                    {
                        robots[i]->crash = true;
                    }
                }
            }
        }
    }

    return crashedRobot;
}

void GameModel::start(string maze)
{
    this->maze = maze;
    // this->maze.resize(MAZE_SIZE);

    remainingDots = 0;
    remainingEnergizers = 0;

    eatenDots = 0;

    for (auto &c : maze)
    {
        if (c == '+')
            remainingDots++;
        else if (c == '#')
            remainingEnergizers++;
    }

    score = 0;
    lives = 4;
    eatenFruits.clear();

    firstBloodFlag = false;

    gameView->start(maze);
    gameView->setScore(score);

    gameState = GameStarting;
    levelMode = NORMAL_MODE;

    gameView->setMessage(GameViewMessageReady);
    gameView->setLives(lives);
    gameView->setEatenFruits(eatenFruits);
    gameView->stopAudio("+");

    for (auto &robot : robots)
        robot->start();
}

void GameModel::update(float deltaTime)
{
    gameStateTime += deltaTime;

    if (waitingForTimeLord && gameStateTime > 1.0f && !((int)gameStateTime % timeLord))
    {
        for (auto &robot : robots)
            robot->forceMove();

        waitingForTimeLord = false;
    }

    if (!waitingForTimeLord)
    {
        if (levelMode == BLINKING_MODE)
        {
            if (gameStateTime > 1.0f && !((int)gameStateTime % 7))
            {
                levelMode = NORMAL_MODE;
                gameView->stopAudio("backgroundEnergizer");

                for (auto &robot : robots)
                    robot->resetTime();
            }

        }

        int crashedRobot = checkRobotCollision();

        if (robots[0]->crash)
        {
            if (levelMode == NORMAL_MODE)
            {
                loseLife();
            }
            else
            {
                robots[0]->crash = false;
                eatEnemy(crashedRobot);
            }
        }

        for (auto &robot : robots)
        {
            robot->setRobotMode(levelMode);
            robot->update(deltaTime);
        }

        if (gameStateTime > 1.0f && !((int)gameStateTime % 3) && !robots[2]->getFree())
            robots[2]->setFree();

        // Para sacar a cyan y orange en función de dots...
        if (!firstBloodFlag)
        {
            if (eatenDots >= 30 && !robots[3]->getFree())
                robots[3]->setFree();

            if (eatenDots >= 60 && !robots[4]->getFree())
                robots[4]->setFree();
        }

        // Para sacar a cyan y orange luego de haber muerto...
        else
        {
            if (gameStateTime > 1.0f && !((int)gameStateTime % 5) && !robots[3]->getFree())
                robots[3]->setFree();

            if (gameStateTime > 1.0f && !((int)gameStateTime % 7) && !robots[4]->getFree())
                robots[4]->setFree();
        }

        if (gameState == GameStarting)
        {
            gameView->stopAudio("backgroundEnergizer");
            gameView->stopAudio("backgroundGhostsCaptured");
            gameView->playAudio("mainStart");

            deployTimeLord(5);
            gameState = GamePlaying;

            for (auto &robot : robots)
                robot->resetTime();
        }

        if(shouldEndLevel())
            newLevel(maze);
    }
}

void GameModel::pickItem(MazePosition *position)
{
    

    char tile = this->maze[position->x + MAZE_WIDTH * position->y];
    if (tile == '+' || tile == '#')
    {
        this->maze[position->x + MAZE_WIDTH * position->y] = ' ';
        gameView->setTiles(position->x, position->y, 0, " ");

        switch (tile)
        {
        case '+':
        {
            remainingDots--;
            eatenDots++;
            score += SCORE_PER_DOT;
            break;
        }
        case '#':
        {
            remainingEnergizers--;
            score += SCORE_PER_ENERGIZER;
            gameStateTime = 0.0f;
            levelMode = BLINKING_MODE;
            gameView->stopAudio("backgroundEnergizer");
            gameView->playAudio("backgroundEnergizer");
            for (int i = 0; i < 4; i++)
                eatenEnemies[i] = false;
            enemyScore = 200;
            break;
        }
        default:
            break;
        }

        gameView->setScore(score);
    }
}

void GameModel::loseLife()
{
    if (--lives)
    {
        for (int i = 0; i < robots.size(); i++)
        {
            robots[i]->start();
        }

        gameState = GameStarting;
        levelMode = NORMAL_MODE;

        gameView->setMessage(GameViewMessageReady);
        gameView->setLives(lives);
        gameView->setEatenFruits(eatenFruits);
    }
    else
    {
        cout << "GAME OVER" << endl;
        cout << "Te dejo seguir jugando pq no se como cerrar el programa nomas" << endl;
    }

    gameView->playAudio("mainLost");
    deployTimeLord(6);
}

void GameModel::eatEnemy(int crashedRobot)
{
    if (!eatenEnemies[crashedRobot - 1])
    {
        eatenEnemies[crashedRobot - 1] = true;

        int eatenEnemiesCount = (int)eatenEnemies[0] + (int)eatenEnemies[1] +
                                (int)eatenEnemies[2] + (int)eatenEnemies[3];

        enemyScore *= 2;
        score += enemyScore;
        gameView->setScore(score);
    }
    robots[crashedRobot]->setRobotMode(RETURN_CAGE);
    gameView->playAudio("eatingGhost");
    gameView->stopAudio("backgroundEnergizer");
    gameView->playAudio("backgroundGhostsCaptured");
    deployTimeLord(6);
}

void GameModel::newLevel(std::string maze)
{
    gameState = GameStarting;

    this->maze = maze;
    // this->maze.resize(MAZE_SIZE);

    remainingDots = 0;
    remainingEnergizers = 0;

    for (auto &c : maze)
    {
        if (c == '+')
            remainingDots++;
        else if (c == '#')
            remainingEnergizers++;
    }

    gameView->start(maze);
    gameView->setScore(score);

    for (auto &robot : robots)
        robot->start();

    gameView->playAudio("mainWon");
}

bool GameModel::shouldEndLevel()
{
    return (remainingDots + remainingEnergizers) == 0; // 200;
}

int GameModel::getLevelMode()
{
    return levelMode;
}

void GameModel::deployTimeLord(int seconds)
{
    timeLord = seconds;
    waitingForTimeLord = true;
    gameStateTime = 0.0f;
}