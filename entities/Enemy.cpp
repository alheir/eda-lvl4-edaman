/**
 * @file Enemy.cpp
 * @authors CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Enemy robot base class
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Enemy.h"
#include "Player.h"
#include "Robot.h"

#include <iostream>

using namespace std;

/**
 * @brief Construct a new Enemy object
 *
 */
Enemy::Enemy()
{
    freeTiles.fill(false);
}

/**
 * @brief Changes enemy's looking and behavior depending on the level mode
 *
 * @param levelMode
 */
void Enemy::setRobotMode(int levelMode)
{
    if (!free && direction)
        levelMode = RETURN_CAGE;

    switch (levelMode)
    {
    case NORMAL_MODE:
    {
        setDisplay(imageIndex);
        setEyes(eyesColor, eyesColor);
        step = 0.6f / 60.0f;
        break;
    }
    case BLINKING_MODE:
    {
        setDisplay(24);
        setEyes(BLUE, BLUE);
        step = 0.4f / 60.0f;
        break;
    }
    case RETURN_CAGE:
    {
        setDisplay(30);
        setEyes(eyesColor, eyesColor);
        step *= 2;
        break;
    }
    }
}

/**
 * @brief Gets the 'behavior mode' depending on the time elapsed
 *
 * @return int
 */
int Enemy::getTimeState()
{
    if (time < 7)
    {
        return DISPERSION;
    }
    else if (time < 27)
    {
        return PERSECUTION;
    }
    else if (time < 34)
    {
        return DISPERSION;
    }
    else if (time < 54)
    {
        return PERSECUTION;
    }
    else if (time < 59)
    {
        return DISPERSION;
    }
    else if (time < 79)
    {
        return PERSECUTION;
    }
    else if (time < 84)
    {
        return DISPERSION;
    }
    else
    {
        return PERSECUTION;
    }
}

/**
 * @brief Updates the enemy
 *
 * @param deltaTime
 */
void Enemy::update(float deltaTime)
{
    time += deltaTime;

    if (!lock)
    {
        mazePosition = getMazePosition(setPoint);
    }

    setRobotMode(gameModel->getLevelMode());
}

/**
 * @brief Moves the enemy
 *
 */
void Enemy::move()
{
    if (!lock)
    {
        if (free)
            findPath(getTargetSetpoint(gameModel->getLevelMode()));
        else if ((mazePosition.x == initialPosition.x) && (mazePosition.y == initialPosition.y))
        {
            direction = 0;
            resetTime();
        }
        else
            findPath(getSetpoint(initialPosition, 0.0f));
    }

    switch (direction)
    {
    case UP:
        setPoint.positionZ += step;
        break;

    case DOWN:
        setPoint.positionZ -= step;
        break;

    case LEFT:
    {
        setPoint.positionX -= step;
        break;
    }
    case RIGHT:
        setPoint.positionX += step;
        break;

    default:
        break;
    }

    setSetpoint(setPoint);

    if (lock)
        lock--;
}

/**
 * @brief Changes the direction of the enemy based on the target setpoint and the
 * current position
 *
 * @param targetSetpoint setpoint to reach
 */
void Enemy::findPath(RobotSetpoint targetSetpoint)
{
    MazePosition targetPosition = getMazePosition(targetSetpoint);
    lock = (int)(0.1f / step);

    if (targetPosition.x == initialPosition.x && targetPosition.y == initialPosition.y)
    {
        start();
    }
    // leave the cage
    else if (mazePosition.y >= 15 && mazePosition.y <= 18)
    {
        if (mazePosition.x >= 11 && mazePosition.x < 13)
        {
            direction = RIGHT;
        }
        else if (mazePosition.x <= 16 && mazePosition.x > 14)
        {
            direction = LEFT;
        }
        else if (mazePosition.x >= 13 && mazePosition.x <= 14)
        {
            direction = UP;
        }
    }
    else
    {
        checkFreeTiles();

        int count = (int)freeTiles[0] + (int)freeTiles[1] +
                    (int)freeTiles[2] + (int)freeTiles[3];

        // Constants DOWN=1, RIGHT=2, UP=3, LEFT=4 are used implicitly in values from variable "i"

        if (count == 0)
            direction = 0;

        else if (count == 1)
        {
            for (int i = 0; i < 4; i++)
            {
                if (freeTiles[i])
                    direction = i + 1;
            }
        }
        else
        {
            float distances[] = {0.0f, 0.0f, 0.0f, 0.0f};
            Vector2 nextStep[] = {{0.0f, -0.1f}, {0.1f, 0.0f}, {0.0f, 0.1f}, {-0.1f, 0.0f}};

            for (int i = 0; i < 4; i++)
            {
                if (freeTiles[i])
                {
                    Vector2 distance = {setPoint.positionX +
                                            nextStep[i].x -
                                            targetSetpoint.positionX,
                                        setPoint.positionZ +
                                            nextStep[i].y -
                                            targetSetpoint.positionZ};

                    distances[i] = (distance.x * distance.x) + (distance.y * distance.y);
                }
            }

            float minDistance = 0.0f;
            for (int i = 0; i < 4; i++)
            {
                if (distances[i] != 0.0f)
                {
                    if ((minDistance == 0.0f) || (distances[i] < minDistance))
                    {
                        minDistance = distances[i];
                        direction = i + 1;
                    }
                }
            }
        }
    }
}

/**
 * @brief Updates contiguous free tiles for the robot
 *
 */
void Enemy::checkFreeTiles()
{
    freeTiles.fill(false);

    // if crashes, don't want to analyze the original direction
    // if doesn't crash, don't want to analyze the opposite direction
    if (((direction != UP) && !crash) || ((direction != DOWN) && crash))
    {
        if (gameModel->isTileFree({mazePosition.x, mazePosition.y + 1}))
        {
            freeTiles[0] = true; // read down
        }
    }
    if (((direction != LEFT) && !crash) || ((direction != RIGHT) && crash))
    {
        if (gameModel->isTileFree({mazePosition.x + 1, mazePosition.y}))
        {
            freeTiles[1] = true; // read right
        }
    }
    if (((direction != DOWN) && !crash) || ((direction != UP) && crash))
    {
        if (gameModel->isTileFree({mazePosition.x, mazePosition.y - 1}))
        {
            freeTiles[2] = true; // read up
        }
    }
    if (((direction != RIGHT) && !crash) || ((direction != LEFT) && crash))
    {
        if (gameModel->isTileFree({mazePosition.x - 1, mazePosition.y}))
        {
            freeTiles[3] = true; // read left
        }
    }

    // variable already used
    crash = false;
}