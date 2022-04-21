/**
 * Robot base class.
 *
 * Copyright (C) 2022 Marc S. Ressl
 */

#ifndef _ROBOT_H
#define _ROBOT_H

class GameModel;

enum DIRECTIONS {DOWN = 1, RIGHT, UP, LEFT};

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
#include "MQTTClient.h"
#include "GameModel.h"

class Robot
{
public:
    Robot();
    virtual ~Robot();

    virtual void start() = 0;
    virtual void update(float deltaTime);
    virtual void resetTime();
    virtual void setRobotMode(int levelMode);

    RobotSetpoint getSetpoint();
    MazePosition getMazePosition();
    int getDirection();
    bool crash;

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
    int direction;

    MazePosition getMazePosition(RobotSetpoint setpoint);
    RobotSetpoint getRobotSetpoint(MazePosition mazePosition, float rotation);
    void setSetpoint(RobotSetpoint setpoint);
    void liftTo(float positionX, float positionZ);
    void setDisplay(int imageIndex);
    void setEyes(Color leftEye, Color rightEye);
    void setDisplayColor(Color color);
};

#endif
