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
    //this->maze.resize(MAZE_SIZE);
    
    remainingDots = 0;
    remainingEnergizers = 0;

    for (auto c : maze)
    {
        if (c == '+')
            remainingDots++;
        else if (c == '#')
            remainingEnergizers++;
    }

    //remainingDots = 20;   // para testear
    score = 0;
    lives = 4;
    eatenFruits.clear();

    gameView->start(maze);
    gameView->setScore(score);

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

int GameModel::refresh(MazePosition* position)
{
    gameState = GamePlaying;
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
                score += 10;
                break;
            }
            case '#':
            {
                remainingEnergizers--;
                score += 50;
                break;
            }
        }
        gameView->setScore(score);
    }

    return remainingDots;
}

void GameModel::newLevel(std::string maze)
{
    this->maze = maze;
    //this->maze.resize(MAZE_SIZE);

    remainingDots = 0;
    remainingEnergizers = 0;

    for (auto c : maze)
    {
        if (c == '+')
            remainingDots++;
        else if (c == '#')
            remainingEnergizers++;
    }

    //remainingDots = 20;   // para testear
    
    gameView->start(maze);
    gameView->setScore(score);

    // Just for testing
    gameView->playAudio("mainStart");
}