// File khai báo (Header): Chứa các nguyên mẫu hàm ghi chú lịch sử hoạt động và cảnh báo của hệ thống.

#ifndef LOG_CONTROL_H
#define LOG_CONTROL_H

#include <Arduino.h>
#include <WiFi.h>

// --- BÁO CHO TRÌNH BIÊN DỊCH BIẾT CÁC HÀM NÀY NẰM Ở FILE .INO ---
extern void blynk_log_event(String event, String msg);
extern void blynk_terminal_print(String msg);

// Hàm ghi log thông thường lên Terminal (chân V2)
void sendSystemLog(String message);

// Hàm ghi log cảnh báo (Gửi Push Notification + Ghi lên Terminal)
void sendAlertLog(String eventName, String message);

#endif