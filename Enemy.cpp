#include "Enemy.h"
#include "Player.h"
#include "Robot.h"

#include <iostream>

using namespace std;

void Enemy::resetTime()
{
    time = 0;
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
        findPath(gameModel->getLevelMode());

    moveEnemy();
}

void Enemy::findPath(RobotSetpoint targetSetpoint)
{
    mazePosition = getMazePosition(setPoint);
    MazePosition mazePosition2 = getMazePosition(targetSetpoint);
    lock = (int) (0.1f / step);

    cout << "Pink en (" << setPoint.positionX << ", " << setPoint.positionZ << ", " << setPoint.rotation << ")" << endl;
    cout << "(" << mazePosition.x << ", " << mazePosition.y << ")" << endl;
    cout << "Player en (" << targetSetpoint.positionX << ", " << targetSetpoint.positionZ << ")" << endl;
    cout << "(" << mazePosition2.x << ", " << mazePosition2.y << ")" << endl;

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
            if(freeTiles[i])
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

    if (direction != UP)
    {
        if (gameModel->isTileFree({ mazePosition.x, mazePosition.y + 1 }))
        {
            freeTiles[0] = true;    // read down
        }
        cout << "read down: " << freeTiles[0] << endl;
    }
    if (direction != LEFT)
    {
        if (gameModel->isTileFree({ mazePosition.x + 1, mazePosition.y }))
        {
            freeTiles[1] = true;    // read right
        }
        cout << "read right: " << freeTiles[1] << endl;
    }
    if (direction != DOWN)
    {
        if (gameModel->isTileFree({ mazePosition.x, mazePosition.y - 1 }))
        {
            freeTiles[2] = true;    // read up
        }
        cout << "read up: " << freeTiles[2] << endl;
    }
    if (direction != RIGHT)
    {
        if (gameModel->isTileFree({ mazePosition.x - 1, mazePosition.y }))
        {
            freeTiles[3] = true;    // read left
        }
        cout << "read left: " << freeTiles[3] << endl;
    }
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