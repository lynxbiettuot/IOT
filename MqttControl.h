#ifndef MQTT_CONTROL_H
#define MQTT_CONTROL_H

#include <PubSubClient.h>

void setupMQTT();
void handleMQTT();
void publishStatus(); // Khai báo để Web đồng bộ

#endif