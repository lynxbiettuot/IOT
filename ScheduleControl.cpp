#include "ScheduleControl.h"
#include "MqttControl.h"
#include <time.h>
#include <Preferences.h>
#include <Arduino.h>

extern void armSystem(bool arm);
Preferences prefs;
bool isScheduleActive = false;
int schH = -1, schM = -1;
bool triggered = false;

void setupNTP() {
  configTime(7 * 3600, 0, "pool.ntp.org", "time.google.com");
  
  prefs.begin("alarm", false);
  isScheduleActive = prefs.getBool("active", false);
  schH = prefs.getInt("h", 0);
  schM = prefs.getInt("m", 0);
  prefs.end();

  isScheduleActive = true; // ÉP NÓ LUÔN BẬT, KHÔNG CẦN LỆNH TỪ WEB NỮA
  
  Serial.println("[NTP] DA EP CHE DO HEN GIO SANG ON!");
}

void toggleSchedule(bool s) {
  isScheduleActive = s;
  prefs.begin("alarm", false);
  prefs.putBool("active", s);
  prefs.end();
  Serial.printf("[MQTT] Da doi trang thai Hen gio: %s\n", s ? "BAT (ON)" : "TAT (OFF)");
}

void setAlarmTime(String t) {
  int col = t.indexOf(':');
  if (col != -1) {
    schH = t.substring(0, col).toInt();
    schM = t.substring(col + 1).toInt();
    prefs.begin("alarm", false);
    prefs.putInt("h", schH); prefs.putInt("m", schM);
    prefs.end();
    Serial.printf("[MQTT] Da nhan gio hen: %02d:%02d\n", schH, schM);
  }
}

void handleSchedule() {
  struct tm ti;
  // Nếu chưa lấy được giờ từ Internet thì báo lỗi
  if (!getLocalTime(&ti)) {
    static unsigned long lastErr = 0;
    if (millis() - lastErr > 5000) {
      Serial.println("-> Dang cho Internet cap nhat gio...");
      lastErr = millis();
    }
    return;
  }

  // --- DÒNG NÀY ĐỂ ÔNG BAT QUA TANG NO DAY ---
  static int lastSec = -1;
  if (ti.tm_sec != lastSec) {
    Serial.printf("Chip hieu: %02d:%02d:%02d | Hen: %02d:%02d | Che do: %s\n", 
                  ti.tm_hour, ti.tm_min, ti.tm_sec, schH, schM, isScheduleActive ? "DANG BAT" : "DANG TAT");
    lastSec = ti.tm_sec;
  }

  // Phải bật chế độ Hẹn giờ trên Web (isScheduleActive) mới kích hoạt
  if (isScheduleActive && ti.tm_hour == schH && ti.tm_min == schM) {
    if (!triggered) {
      Serial.println(">>> KHỚP GIỜ RỒI! DANG KICH HOAT...");
      armSystem(true); 
      triggered = true;
    }
  } else {
    triggered = false;
  }
}