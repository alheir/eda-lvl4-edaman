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

#define STEP 0.01f;
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

    string maze =
        "                            "
        "                            "
        "                            "
        "jbbbbbbbbbbbbonbbbbbbbbbbbbk"
        "s++++++++++++pq++++++++++++r"
        "s+faag+faaag+pq+faaag+faag+r"
        "s#p  q+p   q+pq+p   q+p  q#r"
        "s+v``w+v```w+vw+v```w+v``w+r"
        "s++++++++++++++++++++++++++r"
        "s+faag+fg+faaaaaag+fg+faag+r"
        "s+v``w+pq+v``ih``w+pq+v``w+r"
        "s++++++pq++++pq++++pq++++++r"
        "zccccg+pxaag pq faayq+fcccc{"
        "     s+ph``w vw v``iq+r     "
        "     s+pq          pq+r     "
        "     s+pq dcc__cce pq+r     "
        "jbbbbw+vw r      s vw+vbbbbk"
        "s     +   r      s   +     r"
        "zccccg+fg r      s fg+fcccc{"
        "     s+pq tbbbbbbu pq+r     "
        "     s+pq          pq+r     "
        "     s+pq faaaaaag pq+r     "
        "jbbbbw+vw v``ih``w vw+vbbbbk"
        "s++++++++++++pq++++++++++++r"
        "s+faag+faaag+pq+faaag+faag+r"
        "s+v`iq+v```w+vw+v```w+ph`w+r"
        "s#++pq+++++++  +++++++pq++#r"
        "|ag+pq+fg+faaaaaag+fg+pq+fa}"
        "l`w+vw+pq+v``ih``w+pq+vw+v`m"
        "s++++++pq++++pq++++pq++++++r"
        "s+faaaayxaag+pq+faayxaaaag+r"
        "s+v````````w+vw+v````````w+r"
        "s++++++++++++++++++++++++++r"
        "zcccccccccccccccccccccccccc{"
        "                            "
        "                            ";

    // Setup
    GameModel gameModel(&mqttClient);
    GameView gameView(&mqttClient);

    // Configure
    gameModel.setGameView(&gameView);
    gameModel.start(maze);

    float robot1XZ[] = {0.0f, 0.0f};

    while (!WindowShouldClose() && mqttClient.isConnected())
    {
        float deltaTime = (float)GetFrameTime();

        // raylib
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("EDAPark Controller", 225, 220, 20, LIGHTGRAY);
        EndDrawing();

        vector<MQTTMessage> messages = mqttClient.getMessages();

        // Model update
        gameModel.update(deltaTime);

        // Keyboard control
        
        vector<char> payload = makeMotorPID(robot1XZ[0], robot1XZ[1], 40.0f);
        mqttClient.publish("robot1/pid/setpoint/set", payload);

        // Keyboard control
        if (IsKeyDown(KEY_UP))
        {
            robot1XZ[1] += STEP;   
        }
        else if (IsKeyDown(KEY_RIGHT))
        { 
            robot1XZ[0] += STEP;
        }
        else if (IsKeyDown(KEY_DOWN))
        {
            robot1XZ[1] -= STEP;
        }
        else if (IsKeyDown(KEY_LEFT))
        {
            robot1XZ[0] -= STEP;
        }

        //if (IsKeyPressed(KEY_UP))
        //{
        //    setMotorsDirection(UP_MOVE);
        //}
        //else if (IsKeyPressed(KEY_DOWN))
        //{
        //    setMotorsDirection(DOWN_MOVE);
        //}
        //else if (IsKeyPressed(KEY_RIGHT))
        //{
        //    setMotorsDirection(RIGHT_MOVE);
        //}
        //else if (IsKeyPressed(KEY_LEFT))
        //{
        //    setMotorsDirection(LEFT_MOVE);
        //}
        //else if (IsKeyReleased(KEY_UP) || IsKeyReleased(KEY_DOWN) ||
        //    IsKeyReleased(KEY_RIGHT) || IsKeyReleased(KEY_LEFT))
        //{
        //    setMotorsDirection(STOP);
        //}

        gameView.update(deltaTime);

    }

    CloseWindow();

    cout << "Disconnected." << endl;
}


