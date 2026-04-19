/*
 * Hệ thống chống trộm bằng Loadcell + Blynk IoT
 */

#define BLYNK_TEMPLATE_ID "TMPL6TqH7uSOy"
#define BLYNK_TEMPLATE_NAME "Anti Theft Scale"
#define BLYNK_AUTH_TOKEN "PUTUmi6KlFt1qXsqwbeurgzBAx_ZU7Lu"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "HX711.h"

// --- THÔNG TIN WIFI ---
char ssid[] = "Milo";
char pass[] = "12345622";

// --- KHAI BÁO CHÂN ---
const int LED_RED = 13;
const int LED_GREEN = 12;
const int BUZZER = 27;
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 5;

HX711 scale;
float calibration_factor = -231.15; 
float threshold_weight = 10.0; // Nếu cân nặng dưới 10g coi như bị lấy mất

// --- BIẾN QUẢN LÝ THỜI GIAN ---
unsigned long lastObjectDetectedTime = 0;
bool isAlertActive = false;
const unsigned long delayTime = 2000; // 2 giây

void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  
  Serial.begin(115200);
  
  // Kết nối Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare(); // Lưu ý: Khi khởi động, hãy để vật lên cân trước nếu muốn tare về 0, 
                // hoặc bỏ vật ra để tare về 0 rồi đặt vật sau.
  
  Serial.println("He thong san sang!");
  lastObjectDetectedTime = millis(); // Khởi tạo thời gian
}

void loop() {
  Blynk.run();
  
  if (scale.is_ready()) {
    float weight = scale.get_units(5); // Đọc nhanh 5 mẫu để phản ứng lẹ
    Serial.print("Can nang hien tai: ");
    Serial.println(weight);

    // KIỂM TRA LOGIC
    if (weight >= threshold_weight) {
      // CÒN VẬT: Cập nhật mốc thời gian cuối cùng thấy vật
      lastObjectDetectedTime = millis();
      
      if (isAlertActive) {
        // Nếu đang báo động mà đặt vật lại thì tắt báo động
        stopAlert();
      } else {
        // Trạng thái bình thường
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_RED, LOW);
      }
    } 
    else {
      // MẤT VẬT: Kiểm tra xem đã mất quá 2 giây chưa
      if (millis() - lastObjectDetectedTime > delayTime) {
        if (!isAlertActive) {
          startAlert();
        }
      }
    }
  }
  delay(100); 
}

void startAlert() {
  isAlertActive = true;
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(BUZZER, HIGH);
  
  Serial.println("!!! CANH BAO: DO VAT DA BI LAY TROM !!!");
  
  // Gửi thông báo về app Blynk
  Blynk.logEvent("thief_alert", "Cảnh báo! Đồ vật của bạn đã bị lấy mất quá 2 giây!");
  // Gửi giá trị lên Widget (V1) để hiển thị trạng thái
  Blynk.virtualWrite(V1, "BỊ TRỘM!"); 
}

void stopAlert() {
  isAlertActive = false;
  digitalWrite(BUZZER, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);
  
  Serial.println("Da tra lai vat - Ngung bao dong.");
  Blynk.virtualWrite(V1, "An toàn");
}