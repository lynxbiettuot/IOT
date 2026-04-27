// File chính: Nơi chứa hàm setup() và loop(), quản lý phần cứng (Loadcell HX711, LED, Còi) và logic điều phối trung tâm.

#define BLYNK_TEMPLATE_ID "TMPL6TqH7uSOy"
#define BLYNK_TEMPLATE_NAME "Anti Theft Scale"
#define BLYNK_AUTH_TOKEN "PUTUmi6KlFt1qXsqwbeurgzBAx_ZU7Lu"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "HX711.h"
#include "MqttControl.h"
#include "ScheduleControl.h"
#include "LogControl.h"
#include "TelegramControl.h" // Khai báo module Telegram

// --- KHỞI TẠO TERMINAL Ở FILE CHÍNH ---
WidgetTerminal terminal(V2);

// --- HÀM TRUNG GIAN (WRAPPER) ĐỂ CÁC FILE KHÁC GỌI KÉ BLYNK ---
void blynk_log_event(String event, String msg) {
  Blynk.logEvent(event.c_str(), msg.c_str());
}

void blynk_terminal_print(String msg) {
  terminal.println(msg);
  terminal.flush();
}

const int LED_RED = 13, LED_GREEN = 12, BUZZER = 27;
const int LOADCELL_DOUT_PIN = 4, LOADCELL_SCK_PIN = 5;

HX711 scale;
float calibration_factor = -231.15; 
float threshold_weight = 10.0; 

bool isSystemArmed = true;
bool isAlertActive = false;
unsigned long lastObjectDetectedTime = 0;
const unsigned long delayTime = 2000;

char ssid[] = "VNPT DONG HOANG";
char pass[] = "Nhaproima";

void setup() {
  pinMode(LED_RED, OUTPUT); pinMode(LED_GREEN, OUTPUT); pinMode(BUZZER, OUTPUT);
  Serial.begin(115200);
  
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare(); 

  setupMQTT();
  setupNTP();
  setupTelegram(); 
  
  lastObjectDetectedTime = millis();
  Serial.println("He thong san sang!");
  
  sendTelegramMessage("✅ Hệ thống cân chống trộm ESP32 đã khởi động và kết nối mạng thành công!");
}

void loop() {
  Blynk.run();
  handleMQTT();
  handleSchedule();

  if (isSystemArmed) {
    if (scale.is_ready()) {
      float weight = scale.get_units(5);
      if (weight >= threshold_weight) {
        lastObjectDetectedTime = millis();
        if (isAlertActive) stopAlert();
        else { digitalWrite(LED_GREEN, HIGH); digitalWrite(LED_RED, LOW); }
      } else {
        if (millis() - lastObjectDetectedTime > delayTime) {
          if (!isAlertActive) startAlert();
        }
      }
    }
  } else {
    if (isAlertActive) stopAlert();
    digitalWrite(LED_GREEN, LOW); digitalWrite(LED_RED, LOW);
  }
  delay(100);
}

void startAlert() {
  isAlertActive = true;
  digitalWrite(LED_GREEN, LOW); digitalWrite(LED_RED, HIGH); digitalWrite(BUZZER, HIGH);
  Blynk.virtualWrite(V1, "BỊ TRỘM!");
  
  sendAlertLog("thief_alert", "Vật đã bị lấy khỏi cân!");
}

void stopAlert() {
  isAlertActive = false;
  digitalWrite(BUZZER, LOW); digitalWrite(LED_RED, LOW); digitalWrite(LED_GREEN, HIGH);
  Blynk.virtualWrite(V1, "An toàn");
  
  sendSystemLog("Hệ thống đã về trạng thái An toàn.");
}

void armSystem(bool arm) {
  isSystemArmed = arm;
  lastObjectDetectedTime = millis();
  if (!arm && isAlertActive) stopAlert();
  publishStatus(); 
  
  sendSystemLog(arm ? "Hệ thống BẬT" : "Hệ thống TẮT");
}