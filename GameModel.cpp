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

GameModel::GameModel(MQTTClient* mqttClient)
{
    this->mqttClient = mqttClient;
}

void GameModel::setGameView(GameView* gameView)
{
    this->gameView = gameView;
}

void GameModel::addRobot(Robot* robot)
{
    robots.push_back(robot);
}

bool GameModel::isTileFree(const MazePosition& position)
{
    if ((position.x < 0) || (position.x >= MAZE_WIDTH))
        return false;

    if ((position.y < 0) || (position.y >= MAZE_HEIGHT))
        return false;

    char tile = maze[position.y * MAZE_WIDTH + position.x];

    return (tile == ' ') || (tile == '+') || (tile == '#') || (tile == '_');
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
                        // if enemy is alive
                        if (robots[max(i, j)]->free == true)
                        {
                            robots[0]->crash = true;
                            crashedRobot = max(i, j);
                        }
                    }
                }
                else
                {
                    // avoid collisions between enemies
                    if (((targetPosition.x == mazePosition1.x) && (targetPosition.y == mazePosition1.y)) ||
                        ((targetPosition.x == mazePosition2.x) && (targetPosition.y == mazePosition2.y)))
                    {
                        //if ((robots[i]->free == true) && (robots[j]->free == true))
                        //{
                            robots[i]->crash = true;
                        //}
                        
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

    for (auto c : maze)
    {
        if (c == '+')
            remainingDots++;
        else if (c == '#')
            remainingEnergizers++;
    }

    score = 0;
    lives = 4;
    eatenFruits.clear();

    gameView->start(maze);
    gameView->setScore(score);

    gameState = GameStart;

    /*gameView->setMessage(GameViewMessageReady);
    gameView->setLives(lives);
    gameView->setEatenFruits(eatenFruits);*/

    /*for (auto robot : robots)
        robot->start();*/
}

void GameModel::update(float deltaTime)
{
    if (gameState == GameStart)
    {
        for (auto robot : robots)
        {
            robot->start();
        }
        //eatenDots = 0;
        gameState = GameStarting;
        levelMode = SETUP_MODE;
        gameStateTime = 0;
        freeTimer = 0;
    }
    else if (gameState == GameStarting)
    {
        int readyToStart = 0;       
        for (int i = 1; i < robots.size(); i++)
        {
            robots[i]->update(deltaTime);
            robots[i]->move();
            readyToStart += robots[i]->getDirection();
        }
        if (readyToStart == 0)
        {
            // Just for testing
            gameView->playAudio("mainStart");
            //gameView->playAudio("eatingFruit");
            gameView->setMessage(GameViewMessageReady);
            gameView->setLives(lives);
            gameView->setEatenFruits(eatenFruits);

            WaitTime(6000);
            gameState = GamePlaying;
            gameView->setMessage(GameViewMessageNone);
            levelMode = NORMAL_MODE;

            for (auto robot : robots)
                robot->resetTime();
        }

    }
    else if (gameState == GamePlaying)
    {
        gameStateTime += deltaTime;
        freeTimer += deltaTime;

        if (levelMode == BLINKING_MODE)
        {
            freeTimer = 0;
            if (gameStateTime > 7)
            {
                levelMode = NORMAL_MODE;
                gameStateTime = 0.0f;

                for (auto robot : robots)
                    robot->resetTime();
            }
        }

        enableFree();

        for (auto robot : robots)
        {
            robot->update(deltaTime);
            robot->move();
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
                for (int i = 0; i < 4; i++)
                    eatenEnemies[i] = false;
                enemyScore = 200;
                break;
            }
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
            robots[i]->move();
        }

        gameState = GameStart;

        /*gameView->setMessage(GameViewMessageReady);
        gameView->setLives(lives);
        gameView->setEatenFruits(eatenFruits);*/
    }
    else
    {
        gameState = GameFinish;
        gameView->setMessage(GameViewMessageGameOver);
    }
}

void GameModel::eatEnemy(int crashedRobot)
{
    if (!eatenEnemies[crashedRobot])
    {
        eatenEnemies[crashedRobot] = true;

        //int eatenEnemiesCount = (int)eatenEnemies[0] + (int)eatenEnemies[1] +
        //                        (int)eatenEnemies[2] + (int)eatenEnemies[3];

        enemyScore *= 2;
        score += enemyScore;
        gameView->setScore(score);
    }
    robots[crashedRobot]->free = false;
}

void GameModel::nextScreen(std::string maze)
{
    gameState = GameStart;

    this->maze = maze;
    // this->maze.resize(MAZE_SIZE);

    remainingDots = 0;
    remainingEnergizers = 0;

    for (auto c : maze)
    {
        if (c == '+')
            remainingDots++;
        else if (c == '#')
            remainingEnergizers++;
    }

    gameView->start(maze);

    gameView->playAudio("mainWon");
}

bool GameModel::shouldEndLevel()
{
    return (remainingDots + remainingEnergizers) == 0;
}

int GameModel::getLevelMode()
{
    return levelMode;
}

void GameModel::enableFree()
{   
    if (levelMode != SETUP_MODE)
    {
        for (auto robot : robots)
        {
            if (robot->direction == 0)
            {
                if (eatenDots >= robot->dotsForFree)
                {
                    if (freeTimer >= robot->timeForFree)
                    {
                        robot->free = true;
                    }
                                   
                }
            }        
        }
        
    }
}