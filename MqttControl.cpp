#include "MqttControl.h"
#include "ScheduleControl.h"
#include <WiFi.h>
#include <Arduino.h>

extern bool isSystemArmed;
extern void armSystem(bool arm);

const char* mqtt_server = "broker.hivemq.com";
WiFiClient espClient;
PubSubClient mqttClient(espClient);

void publishStatus() {
  if (mqttClient.connected()) {
    mqttClient.publish("chongtrom/status", isSystemArmed ? "ON" : "OFF");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) msg += (char)payload[i];

  if (msg == "ON") armSystem(true);
  else if (msg == "OFF") armSystem(false);
  else if (msg == "SCH_ON") toggleSchedule(true);
  else if (msg == "SCH_OFF") toggleSchedule(false);
  else if (msg.startsWith("SET_TIME:")) setAlarmTime(msg.substring(9));
}

void setupMQTT() {
  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(callback);
}

void handleMQTT() {
  if (!mqttClient.connected()) {
    if (mqttClient.connect("ESP32_AntiTheft_Client")) {
      mqttClient.subscribe("chongtrom/control");
      publishStatus();
    }
  }
  mqttClient.loop();
}