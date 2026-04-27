

#include "TelegramControl.h"
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <WiFi.h>

// --- THAY THẾ THÔNG TIN CỦA BẠN VÀO 2 DÒNG DƯỚI ĐÂY ---
#define BOT_TOKEN "8652268633:AAHTYK5c6s6dVpGE8zHXlnc5dP6kro_c6BI" 
#define CHAT_ID "8693207646" 

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

void setupTelegram() {
  secured_client.setInsecure(); // Cần thiết để kết nối HTTPS tới Telegram
  Serial.println("[Telegram] Đã khởi tạo kết nối bảo mật.");
}

void sendTelegramMessage(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("[Telegram] Đang gửi tin nhắn...");
    bool isSent = bot.sendMessage(CHAT_ID, message, "");
    if (isSent) {
      Serial.println("[Telegram] Gửi thành công!");
    } else {
      Serial.println("[Telegram] Lỗi: Gửi thất bại.");
    }
  } else {
    Serial.println("[Telegram] Không có WiFi, bỏ qua gửi tin nhắn.");
  }
}