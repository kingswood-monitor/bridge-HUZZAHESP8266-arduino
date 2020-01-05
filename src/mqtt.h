#pragma once

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

namespace mqtt
{
void start(const char *a_clientID, IPAddress address);
void update();
void mqtt_callback(char *topic, byte *payload, unsigned int length);
void publish(const char *topic, const char *payload);
} // namespace mqtt
