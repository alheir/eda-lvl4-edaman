/**
 * EDA-Man game model
 *
 * Copyright (C) 2022 Marc S. Ressl
 *
 * Controls the game model.
 */

#include <iostream>
#include <vector>

#include "GameModel.h"

using namespace std;

const int MAZE_WIDTH = 28;
const int MAZE_HEIGHT = 36;
const int MAZE_SIZE = MAZE_WIDTH * MAZE_HEIGHT;

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

bool GameModel::isTileFree(MazePosition position)
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
    this->maze.resize(MAZE_SIZE);

    for (auto c : maze)
    {
        if (c == '+')
            remainingDots++;
        else if (c == '*')
            remainingEnergizers++;
    }

    gameView->start(maze);

    lives = 4;
    eatenFruits.clear();

    gameState = GameStart;

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

    for (auto robot : robots)
        robot->update(deltaTime);
}
