// File thực thi (Source): Chứa code định dạng thời gian và đẩy log lên Serial Monitor cùng Blynk Terminal.

#include "LogControl.h"
#include <time.h>
#include "TelegramControl.h" // Thêm module Telegram
#include <WiFi.h>

String getCurrentTimeStr() {
  struct tm ti;
  if (!getLocalTime(&ti)) {
    return String(millis() / 1000) + "s"; 
  }
  char timeBuff[50];
  strftime(timeBuff, sizeof(timeBuff), "%H:%M:%S %d/%m/%Y", &ti);
  return String(timeBuff);
}

void sendSystemLog(String message) {
  String timeStamp = "[" + getCurrentTimeStr() + "] ";
  String fullMessage = timeStamp + message;
  
  // Gọi hàm wrapper thay vì gọi terminal.println
  blynk_terminal_print(fullMessage);
  Serial.println("LOG: " + fullMessage);
}

void sendAlertLog(String eventName, String message) {
  // 1. Báo qua Blynk Push Notification bằng hàm wrapper
  blynk_log_event(eventName, message);
  
  // 2. Ghi log Terminal
  sendSystemLog("CẢNH BÁO: " + message);
  
  // 3. BÁO QUA TELEGRAM
  String telegramMsg = "🚨 CẢNH BÁO CHỐNG TRỘM 🚨\n" + message + "\nLúc: " + getCurrentTimeStr();
  sendTelegramMessage(telegramMsg);
}