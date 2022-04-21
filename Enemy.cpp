#include "Enemy.h"
#include "Player.h"
#include "Robot.h"

#include <iostream>

using namespace std;

void Enemy::resetTime()
{
    time = 0;
}

void Enemy::setRobotMode(int levelMode)
{
    switch (levelMode)
    {
        case NORMAL_MODE:
        {
            setDisplay(imageIndex);
            //setDisplayColor(eyesColor);
            setEyes(eyesColor, eyesColor);
            step = 0.1f / 12;
            break;
        }
        case BLINKING_MODE:
        {
            setDisplay(24);
            //setDisplayColor(BLUE);
            setEyes(BLUE, BLUE);
            step = 0.1f / 16;
            break;
        }
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

    if (!lock)
    {
        mazePosition = getMazePosition(setPoint);
    }
}

void Enemy::move()
{
    if (!lock)
    {
        findPath(getTargetSetpoint(gameModel->getLevelMode()));
    }

    moveEnemy();
    if (crash)
        cout << "HOLA ME CHOQUE" << endl;
}

void Enemy::findPath(RobotSetpoint targetSetpoint)
{
    MazePosition targetPosition = getMazePosition(targetSetpoint);
    lock = (int) (0.1f / step);

    /*cout << "Pink en (" << setPoint.positionX << ", " << setPoint.positionZ << ", " << setPoint.rotation << ")" << endl;
    cout << "(" << mazePosition.x << ", " << mazePosition.y << ")" << endl;
    cout << "Player en (" << targetSetpoint.positionX << ", " << targetSetpoint.positionZ << ")" << endl;
    cout << "(" << mazePosition2.x << ", " << mazePosition2.y << ")" << endl;*/

    //if (!crash)
    {
        checkFreeTiles();

        int count = (int)freeTiles[0] + (int)freeTiles[1] + (int)freeTiles[2] + (int)freeTiles[3];

        // Constants DOWN = 1, RIGHT = 2, UP = 3, LEFT = 4 are used implicitly in values from variable "i"

        if (count == 1)
        {
            for (int i = 0; i < 4; i++)
            {
                if (freeTiles[i])
                    direction = i + 1;
            }
        }
        else
        {
            float distances[] = { 0.0f, 0.0f, 0.0f, 0.0f };
            Vector2 nextStep[] = { { 0.0f, -0.1f }, { 0.1f, 0.0f }, { 0.0f, 0.1f }, { -0.1f, 0.0f } };

            for (int i = 0; i < 4; i++)
            {
                if (freeTiles[i])
                {
                    Vector2 distance = { setPoint.positionX + nextStep[i].x - targetSetpoint.positionX,
                                         setPoint.positionZ + nextStep[i].y - targetSetpoint.positionZ };

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
    /*else
    {
        direction = (direction + 1) % 4 + 1;
        crash = false;
    }*/

    switch (direction)
    {
        case DOWN: { cout << "DOWN" << endl; break; }
        case RIGHT: { cout << "RIGHT" << endl; break; }
        case UP: { cout << "UP" << endl; break; }
        case LEFT: { cout << "LEFT" << endl; break; }
    }
}

void Enemy::checkFreeTiles()
{
    freeTiles[0] = false;
    freeTiles[1] = false;
    freeTiles[2] = false;
    freeTiles[3] = false;

    // if crashes, don't want to analyze the original direction
    // if doesn't crash, don't want to analyze the opposite direction
    if (((direction != UP) && !crash) || ((direction != DOWN) && crash))
    {
        if (gameModel->isTileFree({ mazePosition.x, mazePosition.y + 1 }))
        {
            freeTiles[0] = true;    // read down
        }
        cout << "read down: " << freeTiles[0] << endl;
    }
    if (((direction != LEFT) && !crash) || ((direction != RIGHT) && crash))
    {
        if (gameModel->isTileFree({ mazePosition.x + 1, mazePosition.y }))
        {
            freeTiles[1] = true;    // read right
        }
        cout << "read right: " << freeTiles[1] << endl;
    }
    if (((direction != DOWN) && !crash) || ((direction != UP) && crash))
    {
        if (gameModel->isTileFree({ mazePosition.x, mazePosition.y - 1 }))
        {
            freeTiles[2] = true;    // read up
        }
        cout << "read up: " << freeTiles[2] << endl;
    }
    if (((direction != RIGHT) && !crash) || ((direction != LEFT) && crash))
    {
        if (gameModel->isTileFree({ mazePosition.x - 1, mazePosition.y }))
        {
            freeTiles[3] = true;    // read left
        }
        cout << "read left: " << freeTiles[3] << endl;
    }

    crash = false;
}

void Enemy::moveEnemy()
{
    switch (direction)
    {
        case UP:
        {
            setPoint.positionZ += step;
            break;
        }
        case DOWN:
        {
            setPoint.positionZ -= step;
            break;
        }
        case LEFT:
        {
            setPoint.positionX -= step;
            break;
        }
        case RIGHT:
        {
            setPoint.positionX += step;
            break;
        }
    }

    setSetpoint(setPoint);
    lock--;
}

//RobotSetpoint getTargetSetpoint(int levelMode)
//{
//}