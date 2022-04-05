/**
 * Simple MQTT Client
 *
 * Copyright (C) 2022 Marc S. Ressl
 *
 * libmosquitto documentation:
 * https://mosquitto.org/api/files/mosquitto-h.html
 */

#include <cstring>
#include <iostream>

static void onMQTTMessage(struct mosquitto *mosquittoClient,
                          void *context,
                          const struct mosquitto_message *message);

#include "MQTTClient.h"

using namespace std;

// Awful but necessary global variable:
static bool isMosquittoInitialized = false;

// MQTT message callback.
static void onMQTTMessage(struct mosquitto *mosquittoClient,
                          void *context,
                          const struct mosquitto_message *message)
{
    MQTTClient *mqttClient = (MQTTClient *)context;

    MQTTMessage mqttMessage;
    mqttMessage.topic = message->topic;
    mqttMessage.payload.resize(message->payloadlen);
    memcpy(mqttMessage.payload.data(), message->payload, message->payloadlen);

    mqttClient->lastMessages.push_back(mqttMessage);
}

/**
 * Constructs an MQTT client object.
 *
 * @param clientId MQTT client identifier
 */
MQTTClient::MQTTClient(string clientId)
{
    if (!isMosquittoInitialized)
    {
        mosquitto_lib_init();

        isMosquittoInitialized = true;
    }

    const bool cleanSession = true;

    mosquittoInstance = mosquitto_new(clientId.c_str(), cleanSession, this);

    mosquitto_message_callback_set(mosquittoInstance, onMQTTMessage);

    connected = false;
}

MQTTClient::~MQTTClient()
{
    mosquitto_destroy(mosquittoInstance);
}

/**
 * Connects to an MQTT server without encryption.
 *
 * @param host Host to connect to (IP address or domain name)
 * @param host MQTT username
 * @param host MQTT password
 * @return Call successful
 */
bool MQTTClient::connect(string host, int port, string username, string password)
{
    int errorCode;

    mosquitto_username_pw_set(mosquittoInstance,
                              username.c_str(),
                              password.c_str());

    const int keepalive = 60;

    errorCode = mosquitto_connect(mosquittoInstance,
                                  host.c_str(),
                                  port,
                                  keepalive);

    if (errorCode == MOSQ_ERR_SUCCESS)
        connected = true;

    return connected;
}

/**
 * Connection up?
 *
 * @return Connection is up
 */
bool MQTTClient::isConnected()
{
    return connected;
}

/**
 * Disconnects from the MQTT server.
 */
void MQTTClient::disconnect()
{
    mosquitto_disconnect(mosquittoInstance);

    connected = false;
}

/**
 * Publishes an MQTT message on the server.
 *
 * @param topic The MQTT topic
 * @param payload The data to be sent
 * @return Call successful
 */
bool MQTTClient::publish(string topic, vector<char> &payload)
{
    const int qos = 0;
    const bool retain = false;

    int errorCode = mosquitto_publish(mosquittoInstance,
                                      NULL,
                                      topic.c_str(),
                                      (int)payload.size(),
                                      payload.data(),
                                      qos,
                                      retain);

    if (errorCode == MOSQ_ERR_NO_CONN)
        connected = false;

    return (errorCode == MOSQ_ERR_SUCCESS);
}

/**
 * Sends an MQTT subscription request.
 *
 * Topic may contain wildcards ('+' for any topic, '#' for anything below in tree).
 *
 * @param topic The MQTT topic
 * @return Call successful
 */
bool MQTTClient::subscribe(string topic)
{
    const int qos = 0;

    int errorCode = mosquitto_subscribe(mosquittoInstance,
                                        NULL,
                                        topic.c_str(),
                                        qos);

    if (errorCode == MOSQ_ERR_NO_CONN)
        connected = false;

    return (errorCode == MOSQ_ERR_SUCCESS);
}

/**
 * Sends an MQTT unsubscription request.
 *
 * Should match a previous subscription request.
 *
 * @param topic The MQTT topic
 * @return Call successful
 */
bool MQTTClient::unsubscribe(string topic)
{
    int errorCode = mosquitto_unsubscribe(mosquittoInstance,
                                          NULL,
                                          topic.c_str());

    if (errorCode == MOSQ_ERR_NO_CONN)
        connected = false;

    return (errorCode == MOSQ_ERR_SUCCESS);
}

/**
 * Retrieves MQTT messages.
 *
 * Should be called often so messages do not accumulate.
 *
 * @return The MQTT messages
 */
vector<MQTTMessage> MQTTClient::getMessages()
{
    vector<MQTTMessage> messages;

    while (true)
    {
        const int timeout = 0;
        const int maxPackets = 1;

        int errorCode = mosquitto_loop(mosquittoInstance, timeout, maxPackets);

        if ((errorCode == MOSQ_ERR_NO_CONN) ||
            (errorCode == MOSQ_ERR_CONN_LOST))
            connected = false;

        if (!lastMessages.size())
            break;

        // Moves lastMessages to messages
        messages.insert(messages.end(), lastMessages.begin(), lastMessages.end());
        lastMessages.clear();
    }

    return messages;
}
