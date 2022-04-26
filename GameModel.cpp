/**
 * @file GameModel.cpp
 * @authors RESSL ~ CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Controls EDA-Man behavior
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
const int SCORE_PER_FRUIT = 200;

const int MAX_FRUITS = 8;
const int SECS_TO_LAUNCH_A_FRUIT = 5;

/**
 * @brief Construct a new Game Model object
 *
 * @param mqttClient
 */
GameModel::GameModel(MQTTClient *mqttClient)
{
    this->mqttClient = mqttClient;
    highscore = 0;
    run = 1;
}

/**
 * @brief Loads the game-view handler
 *
 * @param gameView
 */
void GameModel::setGameView(GameView *gameView)
{
    this->gameView = gameView;
}

/**
 * @brief Adds a robot to the game model
 *
 * @param robot Address of a robot
 */
void GameModel::addRobot(Robot *robot)
{
    robots.push_back(robot);
}

/**
 * @brief Checks if a given tile is valid to pass through
 *
 * @param position Coordinates of the given tile
 * @return true
 * @return false
 */
bool GameModel::isTileFree(const MazePosition &position)
{
    if ((position.x < 0) || (position.x >= MAZE_WIDTH))
        return false;

    if ((position.y < 0) || (position.y >= MAZE_HEIGHT))
        return false;

    char tile = maze[position.y * MAZE_WIDTH + position.x];

    return (tile == ' ') || (tile == '+') || (tile == '#');
}

/**
 * @brief Initializes the gameModel with a give map
 *
 * @param maze
 */
void GameModel::start(string maze)
{
    this->maze = maze;
    this->mazeBack = maze;

    remainingDots = 0;
    playingEatingDotSound = false;
    playingEndGameSound = false;
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
    fruitTimer = 0;
    fruitActive = false;

    gameView->start(maze);
    gameView->setScore(score);
    gameView->setHighScore(highscore);
    gameView->setRun(run);
    gameView->setEatenFruits(eatenFruits);
    gameView->stopAudio("mainIntermission");
    gameView->playAudio("mainInsertCoin");

    gameState = GameStart;
}

/**
 * @brief Updates de game
 *
 * @param deltaTime
 */
void GameModel::update(float deltaTime)
{
    if (gameState == GameStart)
    {
        for (auto robot : robots)
        {
            robot->start();
            robot->update(deltaTime);
            robot->move();
            WaitTime(255);
        }

        gameView->setMessage(GameViewMessageReady);
        gameView->stopAudio("backgroundEnergizer");
        gameView->playAudio("mainStart");
        gameView->setMessage(GameViewMessageNone);
        gameView->setLives(lives);

        WaitTime(4000);

        gameState = GamePlaying;
        levelMode = NORMAL_MODE;

        for (auto robot : robots)
            robot->resetTime();

        gameStateTime = 0.0f;
        playingEatingDotSound = false;
        playingEndGameSound = false;
    }

    else if (gameState == GamePlaying)
    {
        gameStateTime += deltaTime;

        if (levelMode == BLINKING_MODE)
        {
            if (gameStateTime > 7.0f)
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

        fruitTimer += deltaTime;

        if (fruitTimer >= SECS_TO_LAUNCH_A_FRUIT &&
            !fruitActive &&
            (int)eatenFruits.size() < MAX_FRUITS)
        {
            currentActiveFruitPosition = getRandomFreePosition();
            currentActiveFruitIndex = GetRandomValue(0, 7);

            gameView->setFruit(currentActiveFruitPosition.x,
                               currentActiveFruitPosition.y,
                               currentActiveFruitIndex);

            fruitActive = true;
        }
    }

    else if (gameState == GameFinish)
    {
        if (!playingEndGameSound)
        {
            gameView->stopAudio("backgroundEnergizer");
            gameView->playAudio("mainIntermission");

            if (score > highscore)
            {
                gameView->setHighScore(score);
                highscore = score;
            }

            run++;

            playingEndGameSound = true;
        }
    }

    if (!robots[0]->moving)
    {
        gameView->stopAudio("eatingDots");
        playingEatingDotSound = false;
    }
}

/**
 * @brief Checks collisions between player and enemies, while avoids collisions between enemies.
 *
 * @return int Enemy robot number which collides with player
 */
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
            mazePosition1.y -= 1;
            mazePosition2.y -= 2;
            break;

        case LEFT:
            mazePosition1.x -= 1;
            mazePosition2.x -= 2;
            break;

        case DOWN:
            mazePosition1.y += 1;
            mazePosition2.y += 2;
            break;

        case RIGHT:
            mazePosition1.x += 1;
            mazePosition2.x += 2;
            break;

        default:
            break;
        }

        for (int j = 0; j < robots.size(); j++)
        {
            if (i != j)
            {
                MazePosition targetPosition = robots[j]->getMazePosition();

                if (!i || !j)
                {
                    // detect collision between pacman and enemy
                    if (((targetPosition.x == mazePosition1.x) &&
                         (targetPosition.y == mazePosition1.y)))
                    {
                        // if enemy is alive
                        if (robots[max(i, j)]->free)
                        {
                            robots[0]->crash = true;
                            crashedRobot = max(i, j);
                        }
                    }
                }
                else
                {
                    // avoid collisions between enemies
                    if (((targetPosition.x == mazePosition1.x) &&
                         (targetPosition.y == mazePosition1.y)) ||
                        ((targetPosition.x == mazePosition2.x) &&
                         (targetPosition.y == mazePosition2.y)))
                    {
                        robots[i]->crash = true;
                    }
                }
            }
        }
    }

    return crashedRobot;
}

/**
 * @brief Allows player to eat dots, energizers or fruits.
 *
 * @param position Player's position
 */
void GameModel::pickItem(const MazePosition &position)
{
    char tile = this->maze[position.x + MAZE_WIDTH * position.y];
    if (tile == '+' || tile == '#')
    {
        this->maze[position.x + MAZE_WIDTH * position.y] = ' ';
        gameView->clearDot(position.x, position.y);

        switch (tile)
        {
        case '+':
        {
            remainingDots--;
            eatenDots++;
            score += SCORE_PER_DOT;

            if (!playingEatingDotSound)
            {
                gameView->playAudio("eatingDots");
                playingEatingDotSound = true;
            }

            break;
        }
        case '#':
        {
            remainingEnergizers--;
            score += SCORE_PER_ENERGIZER;
            gameStateTime = 0.0f;
            levelMode = BLINKING_MODE;
            gameView->playAudio("backgroundEnergizer");
            eatenEnemies.fill(false);
            enemyScore = 200;
            enemyScoreIndex = 26;
            break;
        }
        }

        gameView->setScore(score);
    }

    if (fruitActive &&
        position.x == currentActiveFruitPosition.x &&
        position.y == currentActiveFruitPosition.y)
    {
        score += SCORE_PER_FRUIT;

        eatenFruits.push_back(currentActiveFruitIndex);
        fruitActive = false;
        fruitTimer = 0.0f;

        gameView->setEatenFruits(eatenFruits);
        gameView->setTiles(position.x, position.y, 0, " ");
        gameView->playAudio("eatingFruit");
        gameView->setScore(score);
        WaitTime(1000);
    }
}

/**
 * @brief Manages loss of player's life
 *
 */
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
        gameState = GameStart;
        gameView->setLives(lives);
    }
    else
    {
        gameState = GameFinish;
        gameView->setMessage(GameViewMessageGameOver);
    }
}

/**
 * @brief Allows player to eat enemy robots
 *
 * @param crashedRobot Robot to eat
 */
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
        WaitTime(2000);
        gameView->playAudio("backgroundGhostsCaptured");
        gameView->playAudio("backgroundEnergizer");
    }
    robots[crashedRobot]->free = false;
}

/**
 * @brief Sets a new run scene with a given map
 *
 * @param maze
 */
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

/**
 * @brief Checks if the level should end
 *
 * @return true
 * @return false
 */
bool GameModel::shouldEndLevel()
{
    return (remainingDots + remainingEnergizers) == 0;
}

/**
 * @brief Checks if the game is over
 *
 * @return true
 * @return false
 */
bool GameModel::shouldEndGame()
{
    return gameState == GameFinish;
}

/**
 * @brief Returns the current level mode of the game
 *
 * @return int
 */
int GameModel::getLevelMode()
{
    return levelMode;
}

/**
 * @brief Sets robots to free, if applicable.
 *
 */
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

/**
 * @brief Returns a random position on the map, which is passable.
 *
 * @return MazePosition
 */
MazePosition GameModel::getRandomFreePosition()
{
    MazePosition position;

    position.x = GetRandomValue(0, MAZE_HEIGHT - 1);
    position.y = GetRandomValue(0, MAZE_WIDTH - 1);

    // Solo se spawneará una fruta en punto donde hay o hubo dot
    while (mazeBack[position.y * MAZE_WIDTH + position.x] != '+')
    {
        position.x = GetRandomValue(0, MAZE_HEIGHT - 1);
        position.y = GetRandomValue(0, MAZE_WIDTH - 1);
    }

    return position;
}
