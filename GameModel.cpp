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
}

void GameModel::update(float deltaTime)
{
    if (gameState == GameStart)
    {
        for (auto robot : robots)
        {
            robot->start();
            robot->update(deltaTime);
            robot->move();
        }

        gameView->setMessage(GameViewMessageReady);
        gameView->playAudio("mainStart");
        WaitTime(4000);

        gameView->setMessage(GameViewMessageNone);
        gameView->setLives(lives);
        gameView->setEatenFruits(eatenFruits);

        gameState = GamePlaying;
        levelMode = NORMAL_MODE;

        for (auto robot : robots)
            robot->resetTime();

        gameStateTime = 0;
    }

    else if (gameState == GamePlaying)
    {
        gameStateTime += deltaTime;

        if (levelMode == BLINKING_MODE)
        {
            if (gameStateTime > 7)
            {
                levelMode = NORMAL_MODE;
                gameStateTime = 0.0f;
                gameView->stopAudio("backgroundEnergizer");

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
        gameView->clearDot(position->x, position->y);

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
                gameView->playAudio("backgroundEnergizer");
                for (int i = 0; i < 4; i++)
                    eatenEnemies[i] = false;
                enemyScore = 200;
                enemyScoreIndex = 26;
                break;
            }
        }

        gameView->setScore(score);
    }
}

void GameModel::loseLife()
{
    gameView->playAudio("mainLost");
    for (int i = 4; i <= 14; i++)
    {
        robots[0]->setDisplay(i);
        WaitTime(200);
    }

    if (--lives)
    {
        for (int i = 0; i < robots.size(); i++)
        {
            robots[i]->start();
            robots[i]->move();
        }
        gameState = GameStart;
    }
    else
    {
        gameState = GameFinish;
        gameView->setMessage(GameViewMessageGameOver);
    }
}

void GameModel::eatEnemy(int crashedRobot)
{
    if (!eatenEnemies[crashedRobot - 1])
    {
        eatenEnemies[crashedRobot - 1] = true;
        score += enemyScore;
        enemyScore *= 2;
        gameView->setScore(score);

        gameView->stopAudio("backgroundEnergizer");
        gameView->playAudio("eatingGhost");
        robots[crashedRobot]->setDisplay(enemyScoreIndex++);
        WaitTime(1000);
        gameView->playAudio("backgroundGhostsCaptured");
        gameView->playAudio("backgroundEnergizer");
    }
    robots[crashedRobot]->free = false;
}

void GameModel::nextScreen(std::string maze)
{
    gameState = GameStart;

    this->maze = maze;
    gameView->start(maze);
    gameView->playAudio("mainWon");

    eatenDots = 0;
    remainingDots = 0;
    remainingEnergizers = 0;

    for (auto c : maze)
    {
        if (c == '+')
            remainingDots++;
        else if (c == '#')
            remainingEnergizers++;
    }
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
    for (auto robot : robots)
    {
        if (robot->getDirection() == 0)
        {
            if (eatenDots >= robot->dotsForFree)
            {
                if (gameStateTime >= robot->timeForFree)
                {
                    if (levelMode != BLINKING_MODE)
                        robot->free = true;
                }         
            }
        }        
    }
}
