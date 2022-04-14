/**
 * Robot base class.
 *
 * Copyright (C) 2022 Marc S. Ressl
 */

#ifndef _ROBOT_H
#define _ROBOT_H

class GameModel;

enum DIRECTIONS {DOWN = 0, RIGHT, UP, LEFT};

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

    RobotSetpoint getSetpoint();
    MazePosition getMazePosition();

protected:
    // NOTE: These variables should be set by your child class:
    MQTTClient *mqttClient;
    GameModel *gameModel;
    std::string robotId;

    Image displayImages;

    MazePosition mazePosition;
    RobotSetpoint setPoint;
    float step;

    MazePosition getMazePosition(RobotSetpoint setpoint);
    RobotSetpoint getRobotSetpoint(MazePosition mazePosition, float rotation);
    void setSetpoint(RobotSetpoint setpoint);
    void liftTo(float positionX, float positionZ);
    void setDisplay(int imageIndex);
    void setEyes(Color leftEye, Color rightEye);
    void setDisplayColor(Color color);
};

#endif
