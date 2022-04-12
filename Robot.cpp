/**
 * Robot base class.
 *
 * Copyright (C) 2022 Marc S. Ressl
 */

#include <iostream>
#include <vector>
#include <cstring>

#include "Robot.h"
#include "GameModel.h"

using namespace std;

//enum {UP = 1, DOWN, RIGHT, LEFT};

Robot::Robot()
{
    // To-Do: set your path!
    displayImages = LoadImage("../../../RobotImages.png");
    //displayImages = LoadImage("C:/Users/Francisco Mendizabal/source/repos/eda - lvl4 - edaman/RobotImages.png");
}

Robot::~Robot()
{
    UnloadImage(displayImages);
}

Robot1::Robot1(MQTTClient* mqttClient, GameModel* gameModel)
{
    this->mqttClient = mqttClient;
    this->gameModel = gameModel;
    Robot robot1();
}

void Robot1::start()
{
    robotId = "robot1";
    position = { 13, 26 };
    setpoint = { 0.0f, -0.85f, 0.0f };
}

void Robot::update(float deltaTime)
{  
}

// return true if a step began and still didn't end
bool Robot1::move(int direction)
{
    const float STEP = 0.1f;
    const int STEPS = 10;     // changes speed

    const float SEMI_STEP = STEP / STEPS;
    
    bool lock = false;

    if (direction)
    {
        RobotSetpoint auxSetpoint = getRobotSetpoint(position, 0.0f);
        switch (direction)
        {
            case KEY_UP: 
            {
                if (gameModel->isTileFree({position.x, position.y - 1}))
                {
                    if (setpoint.positionZ < (auxSetpoint.positionZ + 0.1f))
                    {
                        setpoint.positionZ += SEMI_STEP;
                        lock = true;
                    }
                    else
                    {
                        position.y--;
                    }
                }
                break;
            }
            case KEY_DOWN:
            {
                if (gameModel->isTileFree({ position.x, position.y + 1 }))
                {
                    if (setpoint.positionZ > (auxSetpoint.positionZ - 0.1f))
                    {
                        setpoint.positionZ -= SEMI_STEP;
                        lock = true;
                    }
                    else
                    {
                        position.y++;
                    }
                }
                break;
            }
            case KEY_RIGHT:
            {
                if (gameModel->isTileFree({ position.x + 1, position.y }))
                {
                    if (setpoint.positionX < (auxSetpoint.positionX + 0.1f))
                    {
                        setpoint.positionX += SEMI_STEP;
                        lock = true;
                    }
                    else
                    {
                        position.x++;
                    }
                }
                break;
            }
            case KEY_LEFT:
            {
                if (gameModel->isTileFree({ position.x - 1, position.y }))
                {
                    if (setpoint.positionX > (auxSetpoint.positionX - 0.1f))
                    {
                        setpoint.positionX -= SEMI_STEP;
                        lock = true;
                    }
                    else
                    {
                        position.x--;
                    }
                }
                break;
            }
            case KEY_R:
            {
                setpoint.rotation += 10.0f;
                break;
            }
        }
    }

    return lock;
}

MazePosition Robot::getMazePosition(RobotSetpoint setpoint)
{
    MazePosition mazePosition;

    mazePosition.x = (int)(10.0F * (1.4F + setpoint.positionX));
    mazePosition.y = (int)(10.0F * (1.8F - setpoint.positionZ));

    return mazePosition;
}

RobotSetpoint Robot::getRobotSetpoint(MazePosition mazePosition, float rotation)
{
    RobotSetpoint setpoint;
    setpoint.positionX = 0.1F * mazePosition.x - 1.35F;
    setpoint.positionZ = 1.75F - 0.1F * mazePosition.y;
    setpoint.rotation = rotation;
    return setpoint;
}


void Robot::setSetpoint(RobotSetpoint setpoint)
{
    vector<char> payload(12);

    *((float *)&payload[0]) = setpoint.positionX;
    *((float *)&payload[4]) = setpoint.positionZ;
    *((float *)&payload[8]) = setpoint.rotation;

    mqttClient->publish(robotId + "/pid/setpoint/set", payload);
}

void Robot::liftTo(float positionX, float positionZ)
{
    vector<char> payload(12);

    *((float *)&payload[0]) = positionX;
    *((float *)&payload[4]) = 0;
    *((float *)&payload[8]) = positionZ;

    mqttClient->publish("hook/" + robotId + "/cmd", payload);
}

void Robot::setDisplay(int imageIndex)
{
    Rectangle selectRectangle = {16.0F * imageIndex, 0, 16, 16};
    Image selectedImage = ImageFromImage(displayImages, selectRectangle);

    const int dataSize = 16 * 16 * 3;
    vector<char> payload(dataSize);
    memcpy(payload.data(), selectedImage.data, dataSize);

    UnloadImage(selectedImage);

    mqttClient->publish(robotId + "/display/lcd/set", payload);
}

void Robot::setEyes(Color leftEye, Color rightEye)
{
    vector<char> payload(3);
    payload[0] = leftEye.r;
    payload[1] = leftEye.g;
    payload[2] = leftEye.b;
    mqttClient->publish(robotId + "/display/leftEye/set", payload);

    payload[0] = rightEye.r;
    payload[1] = rightEye.g;
    payload[2] = rightEye.b;
    mqttClient->publish(robotId + "/display/rightEye/set", payload);
}
