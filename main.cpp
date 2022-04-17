/*
 * EDA-Man
 *
 * Copyright (C) 2022 Marc S. Ressl
 *
 * Controls an EDA-Man game.
 */


//TODO: hacer clientes mqtt para cada robot
//TODO: revisar bien lo hecho hoy (12-04-2022)

#include <iostream>
#include <cstring>

#include <raylib.h>

#include "MQTTClient.h"

#include "GameModel.h"
#include "GameView.h"

#include "Player.h"
#include "Enemy.h"
#include "Red.h"
#include "Pink.h"
#include "Cyan.h"
#include "Orange.h"

#include "Robot.h"

enum KEY_POSITION
{
    STOP = 0,
    UP_MOVE,
    DOWN_MOVE,
    RIGHT_MOVE,
    LEFT_MOVE
};

using namespace std;

vector<char> makeMotorPID(float x, float z, float rotation)
{
    float pidPackage[] = {x, z, rotation};

    std::vector<char> payload;
    payload.resize(sizeof(pidPackage));
    memcpy(payload.data(), &pidPackage, sizeof(pidPackage));

    return (payload);
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
        "                            " // 0
        "                            "
        "                            "
        "jbbbbbbbbbbbbonbbbbbbbbbbbbk"
        "s++++++++++++pq++++++++++++r"
        "s+faag+faaag+pq+faaag+faag+r" // 5
        "s#p  q+p   q+pq+p   q+p  q#r"
        "s+v``w+v```w+vw+v```w+v``w+r"
        "s++++++++++++++++++++++++++r"
        "s+faag+fg+faaaaaag+fg+faag+r"
        "s+v``w+pq+v``ih``w+pq+v``w+r" // 10
        "s++++++pq++++pq++++pq++++++r"
        "zccccg+pxaag pq faayq+fcccc{"
        "     s+ph``w vw v``iq+r     "
        "     s+pq          pq+r     "
        "     s+pq dcc__cce pq+r     " // 15
        "     s+vw r      s vw+r     "
        "     s+   r      s   +r     "
        "     s+fg r      s fg+r     "
        "     s+pq tbbbbbbu pq+r     "
        "     s+pq          pq+r     " // 20
        "     s+pq faaaaaag pq+r     "
        "jbbbbw+vw v``ih``w vw+vbbbbk"
        "s++++++++++++pq++++++++++++r"
        "s+faag+faaag+pq+faaag+faag+r"
        "s+v`iq+v```w+vw+v```w+ph`w+r" // 25
        "s#++pq+++++++  +++++++pq++#r"
        "|ag+pq+fg+faaaaaag+fg+pq+fa}"
        "l`w+vw+pq+v``ih``w+pq+vw+v`m"
        "s++++++pq++++pq++++pq++++++r"
        "s+faaaayxaag+pq+faayxaaaag+r" // 30
        "s+v````````w+vw+v````````w+r"
        "s++++++++++++++++++++++++++r"
        "zcccccccccccccccccccccccccc{"
        "                            "
        "                            "; // 35

    // Setup
    GameModel gameModel(&mqttClient);
    GameView gameView(&mqttClient);

    // Robots
    Player player(&mqttClient, &gameModel);
    Red red(&mqttClient, &gameModel, &player);
    Pink pink(&mqttClient, &gameModel, &player);
    Cyan cyan(&mqttClient, &gameModel, &player, &red);
    Orange orange(&mqttClient, &gameModel, &player);

    gameModel.addRobot(&player);
    gameModel.addRobot(&red);
    gameModel.addRobot(&pink);
    gameModel.addRobot(&cyan);
    gameModel.addRobot(&orange);

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

        player.setDirection(IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT),
                            IsKeyDown(KEY_UP) - IsKeyDown(KEY_DOWN));

        // Updates
        gameModel.update(deltaTime);
        gameView.update(deltaTime);

        if (gameModel.shouldEndLevel())
        {
            gameModel.newLevel(maze);
        }
    }

    CloseWindow();

    cout << "Disconnected." << endl;
}
