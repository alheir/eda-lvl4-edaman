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

Robot::Robot()
{
    displayImages = LoadImage("../RobotImages.png");
}

Robot::~Robot()
{
    UnloadImage(displayImages);
}

void Robot::update(float deltaTime)
{
}

/*
void Robot1::move(GameModel gameModel, MazePosition* position, RobotSetpoint* robotSetpoint)
{
    const float STEP = 0.01f;
    static int direction = 0;
    static bool lock = false;

    //scanKeyboard(mazePosition);

    // Keyboard control
    if (!lock)
    {
        if (IsKeyDown(KEY_UP))
        {
            //cout << "(" << position->x << "," << position->y << ")" << endl;
            position->y -= 1;
            if (gameModel.isTileFree(*position))
            {
                direction = UP;
                lock = true;
            }
            else
                position->y += 1;
        }
        if (IsKeyDown(KEY_DOWN))
        {
            //cout << "(" << position->x << "," << position->y << ")" << endl;
            position->y += 1;
            if (gameModel.isTileFree(*position))
            {
                direction = DOWN;
                lock = true;
            }
            else
                position->y -= 1;
        }
        else if (IsKeyDown(KEY_RIGHT))
        {
            //cout << "(" << position->x << "," << position->y << ")" << endl;
            position->x += 1;
            if (gameModel.isTileFree(*position))
            {
                direction = RIGHT;
                lock = true;
            }
            else
                position->x -= 1;
        }
        else if (IsKeyDown(KEY_LEFT))
        {
            //cout << "(" << position->x << "," << position->y << ")" << endl;
            position->x -= 1;
            if (gameModel.isTileFree(*position))
            {
                direction = LEFT;
                lock = true;
            }
            else
                position->x += 1;
        }
    }
    else
    {
        RobotSetpoint setpoint = getRobotSetpoint(*position, 0.0f);
        //cout << "Comparo " << "(" << robotSetpoint->positionX << ", " << robotSetpoint->positionZ << ")";
        //cout << " con " << "(" << setpoint.positionX << ", " << setpoint.positionZ << ")" << endl;
        switch (direction)
        {
            case UP:
            {
                if (robotSetpoint->positionZ < setpoint.positionZ)
                {
                    robotSetpoint->positionZ += STEP;
                }
                else
                {
                    lock = false;
                }
                break;
            }
            case DOWN:
            {
                if (robotSetpoint->positionZ > setpoint.positionZ)
                {
                    robotSetpoint->positionZ -= STEP;
                }
                else
                {
                    lock = false;
                }
                break;
            }
            case RIGHT:
            {
                if (robotSetpoint->positionX < setpoint.positionX)
                {
                    robotSetpoint->positionX += STEP;
                }
                else
                {
                    lock = false;
                }
                break;
            }
            case LEFT:
            {
                if (robotSetpoint->positionX > setpoint.positionX)
                {
                    robotSetpoint->positionX -= STEP;
                }
                else
                {
                    lock = false;
                }
                break;
            }
        }
    }
    //vector<char> payload = makeMotorPID(robot1XZ.positionX, robot1XZ.positionZ, robot1XZ.rotation);
    //mqttClient.publish("robot1/pid/setpoint/set", payload);
}
*/

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

void Robot::setDisplayColor(Color color)
{
    vector<char> payload(768);

    for(int i = 0; i < 256; i += 3)
    {
        payload[i] = color.r;
        payload[i + 1] = color.g;
        payload[i + 2] = color.b;
    }

    mqttClient->publish(robotId + "/display/lcd/set", payload);
}
