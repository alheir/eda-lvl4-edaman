/**
 * @file Robot.h
 * @authors CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Clase base de robots
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _ROBOT_H
#define _ROBOT_H

class GameModel;

enum DIRECTIONS
{
    DOWN = 1,
    RIGHT,
    UP,
    LEFT
};

struct RobotSetpoint
{
    RobotSetpoint(float positionX = 0, float positionZ = 0, float rotation = 0)
    {
        this->positionX = positionX;
        this->positionZ = positionZ;
        this->rotation = rotation;
    }

    float positionX;
    float positionZ;
    float rotation;
};

#include <string>
#include "../GameModel.h"

class Robot
{
public:
    Robot();
    virtual ~Robot();

    virtual void start() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void move() = 0;
    virtual void setRobotMode(int levelMode) = 0;
    
    void resetTime();
    void forceMove();
    int getDirection();
    RobotSetpoint getSetpoint();
    MazePosition getMazePosition();
    void setDisplay(int imageIndex);

    bool crash;
    bool free;
    int dotsForFree;
    float timeForFree;

    bool moving;

protected:
    MQTTClient *mqttClient;
    GameModel *gameModel;
    std::string robotId;

    Image displayImages;
    int imageIndex;
    Color eyesColor;

    MazePosition mazePosition;
    RobotSetpoint setPoint;
    float step;
    int direction;
    float time;
    
    MazePosition getMazePosition(RobotSetpoint setpoint);
    RobotSetpoint getSetpoint(MazePosition mazePosition, float rotation);
    void setSetpoint(RobotSetpoint setpoint);
    void setEyes(Color leftEye, Color rightEye);
    void setDisplayColor(Color color);

    // we decided not to use it because of bugs and mandatory delays
    void liftTo(float positionX, float positionZ);
};

#endif
