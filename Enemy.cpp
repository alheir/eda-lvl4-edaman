#include "Enemy.h"
#include "Player.h"
#include "Robot.h"

int Enemy::findPath(RobotSetpoint targetSetpoint)
{
    bool freeTiles[] = {false, false, false, false};
    mazePosition = getMazePosition(setPoint);

    if (setPoint.rotation != 0) // up
    {
        if (gameModel->isTileFree({mazePosition.x, mazePosition.y + 1}))
        {
            freeTiles[0] = true;    // read down
        }
    }
    else if (setPoint.rotation != 90) // left
    {
        if (gameModel->isTileFree({mazePosition.x + 1, mazePosition.y}))
        {
            freeTiles[1] = true;    // read right
        }
    }
    else if (setPoint.rotation != 180) // down
    {
        if (gameModel->isTileFree({mazePosition.x, mazePosition.y - 1}))
        {
            freeTiles[2] = true;    // read up
        }
    }
    else if (setPoint.rotation != 270) // right
    {
        if (gameModel->isTileFree({mazePosition.x - 1, mazePosition.y}))
        {
            freeTiles[3] = true;    // read left
        }
    }

    int count = freeTiles[0] + freeTiles[1] + freeTiles[2] + freeTiles[3];
    int direction = -1;
    
    // Constants DOWN = 0, RIGHT = 1, UP = 2, LEFT = 3 are used implicitly in values from variable "i"
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
        // MazePosition auxMazePosition = { mazePosition.x + freeTiles[1] - freeTiles[3] , mazePosition.y + freeTiles[0] - freeTiles[2] };
    }
    else
    {
        float distances[] = {0.0f, 0.0f, 0.0f, 0.0f};
        for (int i = 0; i < 4; i++)
        {
            if(freeTiles[i])
            {
                Vector2 vector = {setPoint.positionX - targetSetpoint.positionX, setPoint.positionZ - targetSetpoint.positionZ};
                distances[i] = (vector.x * vector.x) + (vector.y * vector.y);
            }
        }

        float maxDistance = 0.0f;
        for (int i = 0; i < 4; i++)
        {
            if(distances[i] > maxDistance)
            {    
                maxDistance = distances[i];
                direction = i;
            }
        }
    }
    return direction;
}