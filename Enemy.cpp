#include "Enemy.h"
#include "Player.h"
#include "Robot.h"

#include <iostream>

using namespace std;

int Enemy::findPath(RobotSetpoint targetSetpoint)
{
    bool freeTiles[] = { false, false, false, false };
    mazePosition = getMazePosition(setPoint);
    MazePosition mazePosition2 = getMazePosition(targetSetpoint);

    cout << "Red en (" << setPoint.positionX << ", " << setPoint.positionZ << ", " << setPoint.rotation << ")" << endl;
    cout << "(" << mazePosition.x << ", " << mazePosition.y << ")" << endl;
    cout << "Player en (" << targetSetpoint.positionX << ", " << targetSetpoint.positionZ << ")" << endl;
    cout << "(" << mazePosition2.x << ", " << mazePosition2.y << ")" << endl;

    if (setPoint.rotation != 0) // up
    {
        if (gameModel->isTileFree({mazePosition.x, mazePosition.y + 1}))
        {
            freeTiles[0] = true;    // read down
        }
        cout << "read down: " << freeTiles[0] << endl;
    }
    if (setPoint.rotation != 90) // left
    {
        if (gameModel->isTileFree({mazePosition.x + 1, mazePosition.y}))
        {
            freeTiles[1] = true;    // read right
        }
        cout << "read right: " << freeTiles[1] << endl;
    }
    if (setPoint.rotation != 180) // down
    {
        if (gameModel->isTileFree({mazePosition.x, mazePosition.y - 1}))
        {
            freeTiles[2] = true;    // read up
        }
        cout << "read up: " << freeTiles[2] << endl;
    }
    if (setPoint.rotation != 270) // right
    {
        if (gameModel->isTileFree({mazePosition.x - 1, mazePosition.y}))
        {
            freeTiles[3] = true;    // read left
        }
        cout << "read left: " << freeTiles[3] << endl;
    }

    int count = (int)freeTiles[0] + (int)freeTiles[1] + (int)freeTiles[2] + (int)freeTiles[3];
    int direction = -1;
    
    if (count == 1)
    {
        if(freeTiles[0] == true)
            direction = DOWN;
        else if(freeTiles[1] == true)
            direction = RIGHT;
        else if(freeTiles[2] == true)
            direction = UP;
        else if(freeTiles[3] == true)
            direction = LEFT;

        //cout << direction << endl;
        // MazePosition auxMazePosition = { mazePosition.x + freeTiles[1] - freeTiles[3] , mazePosition.y + freeTiles[0] - freeTiles[2] };
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

        // Constants DOWN = 0, RIGHT = 1, UP = 2, LEFT = 3 are used implicitly in values from variable "i"
        float minDistance = 0.0f;
        for (int i = 0; i < 4; i++)
        {
            if (distances[i] != 0.0f)
            {
                if ((minDistance == 0.0f) || (distances[i] < minDistance))
                {
                    minDistance = distances[i];
                    direction = i;
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
    return direction;
}