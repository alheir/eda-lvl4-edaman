/**
 * EDA-Man game view
 *
 * Copyright (C) 2022 Marc S. Ressl
 *
 * Controls EDD-Man view over MQTT.
 */

#ifndef _GAMEVIEW_H
#define _GAMEVIEW_H

#include <list>

#include "MQTTClient.h"

enum GameViewMessage
{
    GameViewMessageNone,
    GameViewMessageReady,
    GameViewMessageGameOver,
};

/**
 * Controls the EDA-Man views (LED Floor and Jukebox)
 */
class GameView
{
public:
    GameView(MQTTClient *mqttClient);

    void start(std::string maze);
    void update(float time);

    void setMessage(GameViewMessage value);

    void clearDot(int x, int y);
    void setFruit(int x, int y, int fruitIndex);

    void setScore(int value);
    void setHighScore(int value);
    void setLives(int numberOfLives);
    void setEatenFruits(std::list<int> &eatenFruits);

    void setTiles(int x, int y, int palette, std::string s);
    void clearScreen();

    void playAudio(std::string audioId);
    void stopAudio(std::string audioId);

private:
    struct Energizer
    {
        int x;
        int y;
    };

    MQTTClient *mqttClient;

    float time;

    bool blink1Up;
    bool blinkEnergizers;
    std::list<Energizer> energizers;

    GameViewMessage gameViewMessage;
};

#endif
