#include "mqtt.h"

WiFiClient espClient;
PubSubClient client(espClient);

const char *clientID;

namespace mqtt
{
void start(const char *a_clientID, IPAddress address)
{
    clientID = a_clientID;
    client.setServer(address, 1883);
    client.setCallback(mqtt_callback);
}

void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
}

void update()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        if (client.connect(clientID))
        {
            Serial.println("connected");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }

    client.loop();
}

void publish(const char *topic, const char *payload)
{
    client.publish(topic, payload);
}
} // namespace mqtt
