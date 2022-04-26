/**
 * @file Robot.cpp
 * @authors RESSL ~ CATTANEO, HEIR, MENDIZABAL, SCHMUNCK - Grupo 10
 * @brief Clase base de robots
 * @version 0.1
 * @date 2022-04-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "Robot.h"
#include <cstring>

using namespace std;

/**
 * @brief Construct a new Robot object
 *
 */
Robot::Robot()
{
    displayImages = LoadImage("RobotImages.png");
}

/**
 * @brief Destroy the Robot object
 *
 */
Robot::~Robot()
{
    UnloadImage(displayImages);
}

/**
 * @brief Returns robot's setpoint
 *
 * @return RobotSetpoint
 */
RobotSetpoint Robot::getSetpoint()
{
    return setPoint;
}

/**
 * @brief Returns robot's maze position
 *
 * @return MazePosition
 */
MazePosition Robot::getMazePosition()
{
    return mazePosition;
}

/**
 * @brief Returns robot's current direction
 *
 * @return int
 */
int Robot::getDirection()
{
    return direction;
}

/**
 * @brief Resets internal timer
 *
 */
void Robot::resetTime()
{
    time = 0;
}

/**
 * @brief Transforms setpoint to maze position
 *
 * @param setpoint
 * @return MazePosition
 */
MazePosition Robot::getMazePosition(RobotSetpoint setpoint)
{
    MazePosition mazePosition;

    mazePosition.x = (int)(10.0F * (1.4F + setpoint.positionX));
    mazePosition.y = (int)(10.0F * (1.8F - setpoint.positionZ));

    return mazePosition;
}

/**
 * @brief Transforms maze position to setpoint
 *
 * @param mazePosition
 * @param rotation
 * @return RobotSetpoint
 */
RobotSetpoint Robot::getSetpoint(MazePosition mazePosition, float rotation)
{
    RobotSetpoint setpoint;
    setpoint.positionX = 0.1F * mazePosition.x - 1.35F;
    setpoint.positionZ = 1.75F - 0.1F * mazePosition.y;
    setpoint.rotation = rotation;
    return setpoint;
}

/**
 * @brief Sets the robot's setpoint
 *
 * @param setpoint
 */
void Robot::setSetpoint(RobotSetpoint setpoint)
{
    vector<char> payload(12);

    *((float *)&payload[0]) = setpoint.positionX;
    *((float *)&payload[4]) = setpoint.positionZ;
    *((float *)&payload[8]) = setpoint.rotation;

    mqttClient->publish(robotId + "/pid/setpoint/set", payload);
}

/**
 * @brief Lift the robot to a given setpoint
 *
 * @param positionX
 * @param positionZ
 */
void Robot::liftTo(float positionX, float positionZ)
{
    vector<char> payload(12);

    *((float *)&payload[0]) = positionX;
    *((float *)&payload[4]) = 0;
    *((float *)&payload[8]) = positionZ;

    mqttClient->publish("hook/" + robotId + "/cmd", payload);
}

/**
 * @brief Sets de robot's display
 *
 * @param imageIndex Between 0 and 31. See RobotImages.png
 */
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

/**
 * @brief Sets the robot's eyes color
 *
 * @param leftEye
 * @param rightEye
 */
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