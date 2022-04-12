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

int scanKeyboard();

int main(int, char**)
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

    int direction = 0;
    bool lock = false;

    // Robot
    Robot1 robot1(&mqttClient, &gameModel);
    gameModel.addRobot(&robot1);

    // Configure
    gameModel.setGameView(&gameView);
    gameModel.start(maze);

    while (!WindowShouldClose() && mqttClient.isConnected())
    {
        float deltaTime = (float)GetFrameTime();

        // raylib
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("EDAPark Controller", 225, 220, 20, LIGHTGRAY);
        EndDrawing();

        // to lock controls until robot is in the middle of a tile
        if (!lock)
        {
            direction = scanKeyboard();
        }
        lock = robot1.move(direction);
        vector<char> payload = makeMotorPID(robot1.setpoint.positionX, robot1.setpoint.positionZ, robot1.setpoint.rotation);
        mqttClient.publish("robot1/pid/setpoint/set", payload);
        
        if (!gameModel.refresh(robot1.position))    // se podria usar update pero no se si marc queria que la editemos o no
        {
            // new level
            robot1.start();
            gameModel.newLevel(maze);
            vector<char> payload = makeMotorPID(robot1.setpoint.positionX, robot1.setpoint.positionZ, robot1.setpoint.rotation);
            mqttClient.publish("robot1/pid/setpoint/set", payload);
            WaitTime(2000);
        }

        // Updates
        //gameModel.update(deltaTime);  // todavia no hace nada
        //gameView.update(deltaTime);   // todavia no hace nada

    }

    CloseWindow();

    cout << "Disconnected." << endl;
}

int scanKeyboard()
{
    int direction = 0;

    if (IsKeyDown(KEY_UP))
    {
        direction = KEY_UP;
    }
    else if (IsKeyDown(KEY_DOWN))
    {
        direction = KEY_DOWN;
    }
    else if (IsKeyDown(KEY_RIGHT))
    {
        direction = KEY_RIGHT;
    }
    else if (IsKeyDown(KEY_LEFT))
    {
        direction = KEY_LEFT;
    }

    return direction;
}
