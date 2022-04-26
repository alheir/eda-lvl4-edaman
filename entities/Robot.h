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
    virtual void update(float deltaTime);
    virtual void move() = 0;
    virtual void resetTime();
    virtual void setRobotMode(int levelMode);
    void forceMove();

    RobotSetpoint getSetpoint();
    MazePosition getMazePosition();
    int getDirection();
    void setDisplay(int imageIndex);
    bool crash;
    bool free;

    int direction;
    float time;
    unsigned int dotsForFree;
    unsigned int timeForFree;

protected:
    // NOTE: These variables should be set by your child class:
    MQTTClient *mqttClient;
    GameModel *gameModel;
    std::string robotId;

    Image displayImages;
    int imageIndex;
    Color eyesColor;

    MazePosition mazePosition;
    RobotSetpoint setPoint;
    float step;
    
    MazePosition getMazePosition(RobotSetpoint setpoint);
    RobotSetpoint getRobotSetpoint(MazePosition mazePosition, float rotation);
    void setSetpoint(RobotSetpoint setpoint);
    void setEyes(Color leftEye, Color rightEye);
    void setDisplayColor(Color color);

    // we decided not to use it because of bugs and necessary delays
    void liftTo(float positionX, float positionZ);
    
};

#endif
