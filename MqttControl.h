// File khai báo (Header): Chứa các nguyên mẫu hàm cấu hình và xử lý sự kiện giao tiếp MQTT.

#ifndef MQTT_CONTROL_H
#define MQTT_CONTROL_H

#include <PubSubClient.h>

void setupMQTT();
void handleMQTT();
void publishStatus(); // Khai báo để Web đồng bộ

#endif