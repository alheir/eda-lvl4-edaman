/**
 * @file Robot.cpp
 * @authors CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Clase base de robots
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Robot.h"

using namespace std;

Robot::Robot()
{
    displayImages = LoadImage("RobotImages.png");
}

Robot::~Robot()
{
    UnloadImage(displayImages);
}

void Robot::forceMove()
{
    setSetpoint(setPoint);
}

RobotSetpoint Robot::getSetpoint()
{
    return setPoint;
}

MazePosition Robot::getMazePosition()
{
    return mazePosition;
}

int Robot::getDirection()
{
    return direction;
}

void Robot::resetTime()
{
    time = 0;
}

MazePosition Robot::getMazePosition(RobotSetpoint setpoint)
{
    MazePosition mazePosition;

    mazePosition.x = (int)(10.0F * (1.4F + setpoint.positionX));
    mazePosition.y = (int)(10.0F * (1.8F - setpoint.positionZ));

    return mazePosition;
}

RobotSetpoint Robot::getSetpoint(MazePosition mazePosition, float rotation)
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

    for (int i = 0; i < 256; i += 3)
    {
        payload[i] = color.r;
        payload[i + 1] = color.g;
        payload[i + 2] = color.b;
    }

    mqttClient->publish(robotId + "/display/lcd/set", payload);
}
