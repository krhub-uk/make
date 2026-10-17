// Project X robot car — firmware skeleton
// make.krhub.uk  |  MIT licence
//
// What this does today:
//   1. Joins Wi-Fi.
//   2. Reads make.krhub.uk/firmware/version.json.
//   3. If a newer version exists, the car is idle, and USB power is
//      connected, it downloads the new .bin, checks it, and installs it.
//   4. The ESP32 keeps the old firmware in a second slot. If the new one
//      fails to start properly, it rolls back automatically.
//
// Libraries (Arduino IDE, ESP32 core 3.x): WiFi, HTTPClient, Update,
// ArduinoJson (install from Library Manager).

#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <ArduinoJson.h>
#include <esp_ota_ops.h>

#define FW_VERSION   "0.1.0"
#define ROBOT_ID     "kyle-london"   // change per car: ryan, ethan
#define MANIFEST_URL "https://make.krhub.uk/firmware/version.json"

const char* WIFI_SSID = "YOUR_WIFI";
const char* WIFI_PASS = "YOUR_PASSWORD";

// ---- helpers --------------------------------------------------------------

// Compare "1.2.3" style versions. Returns true if remote is newer.
bool isNewer(const char* remote, const char* local) {
  int r[3] = {0}, l[3] = {0};
  sscanf(remote, "%d.%d.%d", &r[0], &r[1], &r[2]);
  sscanf(local,  "%d.%d.%d", &l[0], &l[1], &l[2]);
  for (int i = 0; i < 3; i++) {
    if (r[i] != l[i]) return r[i] > l[i];
  }
  return false;
}

// Placeholders until the motors and USB sensing are wired in.
bool carIsIdle()      { return true; }
bool onUsbPower()     { return true; }

// ---- OTA ------------------------------------------------------------------

void checkForUpdate() {
  HTTPClient http;
  http.begin(MANIFEST_URL);
  if (http.GET() != 200) { http.end(); return; }

  JsonDocument doc;
  if (deserializeJson(doc, http.getString())) { http.end(); return; }
  http.end();

  const char* remoteVer = doc["projectx"]["version"];
  const char* binUrl    = doc["projectx"]["url"];
  if (!remoteVer || !binUrl) return;

  Serial.printf("Running %s, latest %s\n", FW_VERSION, remoteVer);
  if (!isNewer(remoteVer, FW_VERSION)) return;
  if (!carIsIdle() || !onUsbPower()) {
    Serial.println("Update waiting: car must be idle and plugged in.");
    return;
  }

  http.begin(binUrl);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);  // GitHub Releases redirect
  if (http.GET() != 200) { http.end(); return; }

  int len = http.getSize();
  if (!Update.begin(len > 0 ? len : UPDATE_SIZE_UNKNOWN)) { http.end(); return; }
  Update.writeStream(*http.getStreamPtr());
  if (Update.end(true)) {
    Serial.println("Update installed. Restarting.");
    http.end();
    ESP.restart();
  }
  http.end();
}

// After an update, confirm the new firmware is healthy. If this line is
// never reached (crash, boot loop), the bootloader rolls back.
void markFirmwareGood() {
  esp_ota_mark_app_valid_cancel_rollback();
}

// ---- main -----------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  Serial.printf("Project X %s on %s\n", FW_VERSION, ROBOT_ID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  for (int i = 0; i < 40 && WiFi.status() != WL_CONNECTED; i++) delay(250);

  if (WiFi.status() == WL_CONNECTED) {
    markFirmwareGood();
    checkForUpdate();
  }
}

void loop() {
  // Face, tail and drive code arrives in later releases.
  delay(1000);
}
