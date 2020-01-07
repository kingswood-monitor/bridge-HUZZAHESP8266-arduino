/*
 * Kingswood Monitoring System WiFi gateway firmware.
 *  * 
 * Relays data received from a LoRan hub onto the WiFi via MQTT
 * 
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include "utils.h"
#include "sensorjson.h"
#include "mqtt.h"

// Firmware info
#define FIRMWARE_NAME "bridge-HUZZAHESP8266-arduino"
#define FIRMWARE_VERSION "1.1"

// WiFi credentials
#define SSID_NAME "Kingswood"
#define SSID_PASS "wbtc0rar"
#define MQTT_SERVER_IP IPAddress(192, 168, 1, 200)
#define BLINK_AUTH "zMkMyutAQEtOG6aU8Y-ELB4AI4PkxGav" // Project "Outside"

// Hardware configuration
#define LED_RED 0
#define LED_BLUE 2
#define SERIAL_PIN 13                      // IO13 / RXD2
SoftwareSerial SoftSerial(SERIAL_PIN, -1); // RX, TX

char *DEVICE_ID;

// Define the JSON doc
// See structure in feather32u4_rfm9x-arduino-lora433TX and https://arduinojson.org/v6/assistant/ for capacity
const size_t capacity = JSON_OBJECT_SIZE(1) + 2 * JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 110;

void updateBlynk(StaticJsonDocument<capacity> *doc);

void setup()
{
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_RED, HIGH); // Off

  DEVICE_ID = util::deviceID("32U4ESP8266WIFI");

  SoftSerial.begin(9600);
  Serial.begin(115200);
  delay(1000);

  util::printBanner(FIRMWARE_NAME, FIRMWARE_VERSION, DEVICE_ID);

  mqtt::start(DEVICE_ID, MQTT_SERVER_IP);
  Blynk.begin(BLINK_AUTH, SSID_NAME, SSID_PASS);
}

void loop()
{
  Blynk.run();
  mqtt::update();

  if (!SoftSerial.available())
    return;

  char packet[512] = {0};
  SoftSerial.readBytes(packet, 512);
  StaticJsonDocument<capacity> doc;
  DeserializationError error = deserializeJson(doc, packet);
  if (error)
  {
    Serial.print("deserializeJson() failed with code ");
    Serial.println(error.c_str());
    return;
  }

  TopicVisitor visitor = TopicVisitor();
  doc.accept(visitor);
  for (auto event : visitor.getData())
  {
    mqtt::publish(event.topic.c_str(), event.data.c_str());
  }

  updateBlynk(&doc);

  digitalWrite(LED_RED, LOW);
  delay(100);
  digitalWrite(LED_RED, HIGH);
}

void updateBlynk(StaticJsonDocument<capacity> *doc)
{
  // const char *deviceID = (*doc)["deviceID"];
  long int packetID = (*doc)["packetID"];
  float SHT15_temperature = (*doc)["sensors"]["SHT15"]["temperature"];
  float SHT15_humidity = (*doc)["sensors"]["SHT15"]["humidity"];
  long int status_lora_packetRssi = (*doc)["status"]["lora"]["packetRssi"];
  long int status_lora_packetSnr = (*doc)["status"]["lora"]["packetSnr"];
  long int status_lora_packetFrequencyError = (*doc)["status"]["lora"]["packetFrequencyError"];
  float status_batteryVoltage = (*doc)["status"]["batteryVoltage"];
  // const char *status_info = (*doc)["status"]["info"];

  Blynk.virtualWrite(V1, packetID);
  Blynk.virtualWrite(V2, util::round_to_digits_s(SHT15_temperature, 1));
  Blynk.virtualWrite(V3, util::round_to_digits_s(SHT15_humidity, 0));
  Blynk.virtualWrite(V4, status_lora_packetRssi);
  Blynk.virtualWrite(V5, status_lora_packetSnr);
  Blynk.virtualWrite(V6, status_lora_packetFrequencyError);
  Blynk.virtualWrite(V7, util::round_to_digits_s(status_batteryVoltage, 1));
}