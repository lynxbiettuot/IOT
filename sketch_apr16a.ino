#define BLYNK_TEMPLATE_ID "TMPL6TqH7uSOy"
#define BLYNK_TEMPLATE_NAME "Anti Theft Scale"
#define BLYNK_AUTH_TOKEN "PUTUmi6KlFt1qXsqwbeurgzBAx_ZU7Lu"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "HX711.h"
#include "MqttControl.h"
#include "ScheduleControl.h"

// Cấu hình chân và biến
const int LED_RED = 13, LED_GREEN = 12, BUZZER = 27;
const int LOADCELL_DOUT_PIN = 4, LOADCELL_SCK_PIN = 5;

HX711 scale;
float calibration_factor = -231.15; 
float threshold_weight = 10.0; 

bool isSystemArmed = true;
bool isAlertActive = false;
unsigned long lastObjectDetectedTime = 0;
const unsigned long delayTime = 2000;

char ssid[] = "Milo";
char pass[] = "12345622";

void setup() {
  pinMode(LED_RED, OUTPUT); pinMode(LED_GREEN, OUTPUT); pinMode(BUZZER, OUTPUT);
  Serial.begin(115200);
  
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare(); 

  setupMQTT();
  setupNTP();
  
  lastObjectDetectedTime = millis();
  Serial.println("He thong san sang!");
}

void loop() {
  Blynk.run();
  handleMQTT();
  handleSchedule();

  if (isSystemArmed) {
    if (scale.is_ready()) {
      float weight = scale.get_units(5);
      if (weight >= threshold_weight) {
        lastObjectDetectedTime = millis(); // Luôn cập nhật nếu còn vật
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
  Blynk.logEvent("thief_alert", "Cảnh báo! Vật đã bị lấy!");
  Blynk.virtualWrite(V1, "BỊ TRỘM!");
}

void stopAlert() {
  isAlertActive = false;
  digitalWrite(BUZZER, LOW); digitalWrite(LED_RED, LOW); digitalWrite(LED_GREEN, HIGH);
  Blynk.virtualWrite(V1, "An toàn");
}

// Hàm bổ trợ để bật hệ thống an toàn từ các file khác
void armSystem(bool arm) {
  isSystemArmed = arm;
  lastObjectDetectedTime = millis(); // Reset mốc thời gian để tránh báo động giả
  if (!arm && isAlertActive) stopAlert();
  publishStatus(); // Báo về Web
}