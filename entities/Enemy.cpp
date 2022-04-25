/**
 * @file Enemy.cpp
 * @authors CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Clase base de robots enemigos
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

static const int blueModeFrameIndex = 24;
static const int eyesModeFrameIndex = 30;

Enemy::Enemy()
{
    player = NULL;
    time = 0;
    robotState = 0;
    lock = 0;
    freeTiles.fill(false);
}

void Enemy::resetTime()
{
    time = 0;
}

void Enemy::setRobotMode(int levelMode)
{
    robotState = levelMode;

    switch (levelMode)
    {
    case NORMAL_MODE:
        setDisplay(imageIndex);
        setEyes(eyesColor, eyesColor);
        step = 0.6f / 60.0f;
        break;

    case BLINKING_MODE:
        setDisplay(blueModeFrameIndex);
        setEyes(BLUE, BLUE);
        step = 0.4f / 60.0f;
        break;

    case RETURN_CAGE:
        free = false;
        setDisplay(eyesModeFrameIndex);
        setEyes(eyesColor, eyesColor);
        //step = 0.1f / 8;
        setPoint = {0.0f, 0.0f, 0.0f};
        mazePosition = getMazePosition(setPoint);
        liftTo(0.0f, 0.0f);
        forceMove();
        lock = 0;
        free = true;
        break;

    default:
        break;
    }
}

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

void Enemy::update(float deltaTime)
{
    time += deltaTime;

    // Búsqueda de la siguiente dirección
    if (!lock)
    {
        mazePosition = getMazePosition(setPoint);

        if (free)
            findPath(getTargetSetpoint(gameModel->getLevelMode()));
    }

    if (free)
    {
        switch (direction)
        {
        case UP:
            setPoint.positionZ += step;
            break;

        case DOWN:
            setPoint.positionZ -= step;
            break;

        case LEFT:
            setPoint.positionX -= step;
            break;

        case RIGHT:
            setPoint.positionX += step;
            break;

        default:
            break;
        }

        setSetpoint(setPoint);

        lock--;
    }
}

void Enemy::findPath(RobotSetpoint targetSetpoint)
{
    // gameModel->gameView->setTiles(getMazePosition(targetSetpoint).x, getMazePosition(targetSetpoint).y, 1, "x");

    MazePosition targetPosition = getMazePosition(targetSetpoint);
    lock = (int)(0.1f / step);

    /*cout << "Pink en (" << setPoint.positionX << ", " << setPoint.positionZ << ", " << setPoint.rotation << ")" << endl;
    cout << "(" << mazePosition.x << ", " << mazePosition.y << ")" << endl;
    cout << "Player en (" << targetSetpoint.positionX << ", " << targetSetpoint.positionZ << ")" << endl;
    cout << "(" << mazePosition2.x << ", " << mazePosition2.y << ")" << endl;*/

    if (mazePosition.y >= 15 && mazePosition.y <= 18)
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

        int count = (int)freeTiles[0] + (int)freeTiles[1] + (int)freeTiles[2] + (int)freeTiles[3];

        // Constants DOWN = 1, RIGHT = 2, UP = 3, LEFT = 4 are used implicitly in values from variable "i"

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
                    Vector2 distance = {setPoint.positionX + nextStep[i].x - targetSetpoint.positionX,
                                        setPoint.positionZ + nextStep[i].y - targetSetpoint.positionZ};

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

    /*switch (direction)
    {
        case DOWN: { cout << "DOWN" << endl; break; }
        case RIGHT: { cout << "RIGHT" << endl; break; }
        case UP: { cout << "UP" << endl; break; }
        case LEFT: { cout << "LEFT" << endl; break; }
    }*/

    switch (direction)
    {
    case DOWN:
        setPoint.rotation = 180.0f;
        break;
    case RIGHT:
        setPoint.rotation = 90.0f;
        break;
    case UP:
        setPoint.rotation = 0.0f;
        break;
    case LEFT:
        setPoint.rotation = 270.0f;
        break;
    default:
        break;
    }
}

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
        cout << "read down: " << freeTiles[0] << endl;
    }
    if (((direction != LEFT) && !crash) || ((direction != RIGHT) && crash))
    {
        if (gameModel->isTileFree({mazePosition.x + 1, mazePosition.y}))
        {
            freeTiles[1] = true; // read right
        }
        cout << "read right: " << freeTiles[1] << endl;
    }
    if (((direction != DOWN) && !crash) || ((direction != UP) && crash))
    {
        if (gameModel->isTileFree({mazePosition.x, mazePosition.y - 1}))
        {
            freeTiles[2] = true; // read up
        }
        cout << "read up: " << freeTiles[2] << endl;
    }
    if (((direction != RIGHT) && !crash) || ((direction != LEFT) && crash))
    {
        if (gameModel->isTileFree({mazePosition.x - 1, mazePosition.y}))
        {
            freeTiles[3] = true; // read left
        }
        cout << "read left: " << freeTiles[3] << endl;
    }

    crash = false;
}