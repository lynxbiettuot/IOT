// File khai báo (Header): Chứa các nguyên mẫu hàm đồng bộ thời gian NTP và quản lý lịch trình bật/tắt.

#ifndef SCHEDULE_CONTROL_H
#define SCHEDULE_CONTROL_H
#include <Arduino.h>

void setupNTP();
void handleSchedule();
void setAlarmTime(String t);
void toggleSchedule(bool s);

#endif