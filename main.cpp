/*
 * EDA-Man
 *
 * Copyright (C) 2022 Marc S. Ressl
 *
 * Controls an EDA-Man game.
 */

#include <iostream>
#include <cstring>

#include <raylib.h>

#include "MQTTClient.h"

#include "GameModel.h"
#include "GameView.h"

// no deberia ir aca supongo pero por ahora si
#include "Robot.h"

enum KEY_POSITION { STOP = 0, UP_MOVE, DOWN_MOVE, RIGHT_MOVE, LEFT_MOVE };

using namespace std;

vector<char> makeMotorPID(float x, float z, float rotation)
{
    float pidPackage[] = {x, z, rotation};

    std::vector<char> payload;
    payload.resize(sizeof(pidPackage));
    memcpy(payload.data(), &pidPackage, sizeof(pidPackage));

    return(payload);
}


int main(int, char **)
{
    MQTTClient mqttClient("controller");

    const int port = 1883;
    if (!mqttClient.connect("localhost", port, "user", "vdivEMMN3SQWX2Ez"))
    {
        cout << "Could not connect." << endl;
        return 1;
    }

    cout << "Connected." << endl;

    // raylib
    InitWindow(640, 480, "EDA-Man Controller");
    SetTargetFPS(60);

    // 28 columns (0-27)
    string maze =
        "                            "  // 0
        "                            "
        "                            "
        "jbbbbbbbbbbbbonbbbbbbbbbbbbk"
        "s++++++++++++pq++++++++++++r"
        "s+faag+faaag+pq+faaag+faag+r"  // 5
        "s#p  q+p   q+pq+p   q+p  q#r"
        "s+v``w+v```w+vw+v```w+v``w+r"
        "s++++++++++++++++++++++++++r"
        "s+faag+fg+faaaaaag+fg+faag+r"
        "s+v``w+pq+v``ih``w+pq+v``w+r"  // 10
        "s++++++pq++++pq++++pq++++++r"
        "zccccg+pxaag pq faayq+fcccc{"
        "     s+ph``w vw v``iq+r     "
        "     s+pq          pq+r     "
        "     s+pq dcc__cce pq+r     "  // 15
        "jbbbbw+vw r      s vw+vbbbbk"
        "s     +   r      s   +     r" 
        "zccccg+fg r      s fg+fcccc{"
        "     s+pq tbbbbbbu pq+r     "
        "     s+pq          pq+r     "  // 20
        "     s+pq faaaaaag pq+r     "
        "jbbbbw+vw v``ih``w vw+vbbbbk"
        "s++++++++++++pq++++++++++++r"
        "s+faag+faaag+pq+faaag+faag+r"
        "s+v`iq+v```w+vw+v```w+ph`w+r"  // 25
        "s#++pq+++++++  +++++++pq++#r"
        "|ag+pq+fg+faaaaaag+fg+pq+fa}"
        "l`w+vw+pq+v``ih``w+pq+vw+v`m"
        "s++++++pq++++pq++++pq++++++r"
        "s+faaaayxaag+pq+faayxaaaag+r"  // 30
        "s+v````````w+vw+v````````w+r"
        "s++++++++++++++++++++++++++r"
        "zcccccccccccccccccccccccccc{"
        "                            "
        "                            "; // 35

    // Setup
    GameModel gameModel(&mqttClient);
    GameView gameView(&mqttClient);

    // Robot
    Robot1 robot1;
    gameModel.addRobot(&robot1);
    MazePosition position = { 13, 26 };
    RobotSetpoint robotSetpoint = { 0.0f, -0.85f, 0.0f };

    // Configure
    gameModel.setGameView(&gameView);
    gameModel.start(maze);
    
    /*vector<char> payload = makeMotorPID(setpoint.positionX, setpoint.positionZ, setpoint.rotation);
    mqttClient.publish("robot1/pid/setpoint/set", payload);*/

    while (!WindowShouldClose() && mqttClient.isConnected())
    {
        float deltaTime = (float)GetFrameTime();

        // raylib
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("EDAPark Controller", 225, 220, 20, LIGHTGRAY);
        EndDrawing();

        //vector<MQTTMessage> messages = mqttClient.getMessages();
        
        robot1.move(gameModel, &position, &robotSetpoint);
        vector<char> payload = makeMotorPID(robotSetpoint.positionX, robotSetpoint.positionZ, robotSetpoint.rotation);
        mqttClient.publish("robot1/pid/setpoint/set", payload);

        if (!gameModel.refresh(&position))
        {
            // new level
            position = { 13, 26 };
            robotSetpoint = { 0.0f, -0.85f, 0.0f };
            gameModel.newLevel(maze);
            vector<char> payload = makeMotorPID(robotSetpoint.positionX, robotSetpoint.positionZ, robotSetpoint.rotation);
            mqttClient.publish("robot1/pid/setpoint/set", payload);
            WaitTime(3000);
        }

        // Updates
        //gameModel.update(deltaTime);  // todavia no hace nada
        //gameView.update(deltaTime);   // todavia no hace nada

    }

    CloseWindow();

    cout << "Disconnected." << endl;
}


