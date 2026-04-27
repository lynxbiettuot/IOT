// File khai báo (Header): Chứa các nguyên mẫu hàm khởi tạo kết nối bảo mật và gửi tin nhắn Telegram.

#ifndef TELEGRAM_CONTROL_H
#define TELEGRAM_CONTROL_H

#include <Arduino.h>
#include <WiFi.h>

void setupTelegram();
void sendTelegramMessage(String message);

#endif