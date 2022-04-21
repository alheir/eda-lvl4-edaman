/**
 * EDA-Man game model
 *
 * Copyright (C) 2022 Marc S. Ressl
 *
 * Controls the game model.
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

    /*for (int i = 1; i < robots.size(); i++)
    {
        MazePosition mazePosition = robots[i]->getMazePosition();
        if ((position.x == mazePosition.x) && (position.y == mazePosition.y))
            return false;
        else
            switch (direction)
            {
                case UP:
                {
                    if ((position.x == mazePosition.x) && (position.y == mazePosition.y + 1))
                        return false;
                    break;
                }
                case LEFT:
                {
                    if ((position.x == mazePosition.x + 1) && (position.y == mazePosition.y))
                        return false;
                    break;
                }
                case DOWN:
                {
                    if ((position.x == mazePosition.x) && (position.y == mazePosition.y - 1))
                        return false;
                    break;
                }
                case RIGHT:
                {
                    if ((position.x == mazePosition.x - 1) && (position.y == mazePosition.y))
                        return false;
                    break;
                }
            }
    }*/

    return (tile == ' ') || (tile == '+') || (tile == '#');
}

void GameModel::checkRobotCollision()
{
    for (int i = 1; i < robots.size(); i++)
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

                if (((targetPosition.x == mazePosition1.x) && (targetPosition.y == mazePosition1.y)) ||
                    ((targetPosition.x == mazePosition2.x) && (targetPosition.y == mazePosition2.y)))
                {
                    robots[i]->crash = true;
                }
            }
        }

        cout << robots[i]->crash;
    }
    cout << endl;
}

void GameModel::start(string maze)
{
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

    score = 0;
    lives = 4;
    eatenFruits.clear();

    gameView->start(maze);
    gameView->setScore(score);

    gameState = GameStarting;
    levelMode = NORMAL_MODE;

    gameView->setMessage(GameViewMessageReady);
    gameView->setLives(lives);
    gameView->setEatenFruits(eatenFruits);

    for (auto robot : robots)
        robot->start();

    // Just for testing
    gameView->playAudio("mainStart");
}

void GameModel::update(float deltaTime)
{
    gameStateTime += deltaTime;

    if (levelMode == BLINKING_MODE)
    {
        if (gameStateTime > 7)
        {
            levelMode = NORMAL_MODE;
            gameStateTime = 0.0f;

            for (auto robot : robots)
                robot->resetTime();
        }
    }

    for (auto robot : robots)
    {
        int direction = robot->getDirection();        
        robot->update(deltaTime);
    }

    checkRobotCollision();

    for (auto robot : robots)
    {
        robot->move();
        robot->setRobotMode(levelMode);
    }

    cout << endl;
    WaitTime(10);
        
    if (gameState == GameStarting)
    {
        WaitTime(4000);
        gameState = GamePlaying;

        for (auto robot : robots)
            robot->resetTime();
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
            score += SCORE_PER_DOT;
            break;
        }
        case '#':
        {
            remainingEnergizers--;
            score += SCORE_PER_ENERGIZER;
            gameStateTime = 0.0f;
            levelMode = BLINKING_MODE;
            break;
        }

        default:
            break;
        }

        gameView->setScore(score);
    }
}

void GameModel::newLevel(std::string maze)
{
    gameState = GameStarting;

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
    gameView->setScore(score);

    for (auto robot : robots)
        robot->start();

    // Just for testing
    gameView->playAudio("mainStart");
}

bool GameModel::shouldEndLevel()
{
    return (remainingDots + remainingEnergizers) == 0; //200;
}

int GameModel::getLevelMode()
{
    return levelMode;
}