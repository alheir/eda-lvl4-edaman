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
        robot->update(deltaTime);
        robot->setRobotMode(levelMode);
    }
        
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