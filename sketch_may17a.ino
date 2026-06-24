#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "DHT.h"

#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

// ======================= HARDWARE =======================

// DHT22
#define DHTPIN 33
#define DHTTYPE DHT22

// RELAYS (ACTIVE LOW)
#define FAN_PIN 25

#define PUMP1_PIN 14
#define PUMP2_PIN 27
#define PUMP3_PIN 26

// SOIL SENSORS
#define SOIL1_PIN 32
#define SOIL2_PIN 35
#define SOIL3_PIN 34

// ======================= CALIBRATION =======================
#define WET_VALUE 1500
#define DRY_VALUE 4095

// ======================= WIFI / FIREBASE =======================
#define WIFI_SSID "Greenhouse"
#define WIFI_PASSWORD "123123123"

#define API_KEY "AIzaSyDXJOmWNtAeXbt9O_2frSS1c_0W-DPSuQM"
#define DATABASE_URL "https://greenhouse-54a64-default-rtdb.firebaseio.com/"

// ======================= OBJECTS =======================
DHT dht(DHTPIN, DHTTYPE);

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long lastSend = 0;
const unsigned long interval = 5000;

// ======================= RELAY CONTROL =======================

void fanOn()  { digitalWrite(FAN_PIN, LOW);  Serial.println("FAN ON"); }
void fanOff() { digitalWrite(FAN_PIN, HIGH); Serial.println("FAN OFF"); }

void pumpOn(int pin, String name) {
  digitalWrite(pin, LOW);
  Serial.println(name + " ON");
}

void pumpOff(int pin, String name) {
  digitalWrite(pin, HIGH);
  Serial.println(name + " OFF");
}

// ======================= SOIL % =======================
int getSoilPercent(int pin) {
  int raw = analogRead(pin);

  int percent = map(raw, DRY_VALUE, WET_VALUE, 0, 100);
  return constrain(percent, 0, 100);
}

// ======================= SETUP =======================
void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(FAN_PIN, OUTPUT);
  pinMode(PUMP1_PIN, OUTPUT);
  pinMode(PUMP2_PIN, OUTPUT);
  pinMode(PUMP3_PIN, OUTPUT);

  // SAFE OFF
  fanOff();
  pumpOff(PUMP1_PIN, "PUMP1");
  pumpOff(PUMP2_PIN, "PUMP2");
  pumpOff(PUMP3_PIN, "PUMP3");

  // WIFI
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nWiFi Connected");

  // FIREBASE
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase OK");
  } else {
    Serial.printf("Firebase Error: %s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

// ======================= LOOP =======================
void loop() {

  if (!Firebase.ready()) return;

  if (millis() - lastSend < interval) return;
  lastSend = millis();

  // ================= DHT =================
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT ERROR");
    return;
  }

  // ================= SOIL =================
  int soil1 = getSoilPercent(SOIL1_PIN);
  int soil2 = getSoilPercent(SOIL2_PIN);
  int soil3 = getSoilPercent(SOIL3_PIN);

  // ================= PRINT =================
  Serial.println("\n===== GREENHOUSE =====");
  Serial.printf("Temp: %.2f C\n", temp);
  Serial.printf("Humidity: %.2f %%\n", hum);
  Serial.printf("Soil1: %d %%\n", soil1);
  Serial.printf("Soil2: %d %%\n", soil2);
  Serial.printf("Soil3: %d %%\n", soil3);

  // ================= UPLOAD =================
  FirebaseJson json;

  json.set("temperature", temp);
  json.set("humidity", hum);
  json.set("soil1", soil1);
  json.set("soil2", soil2);
  json.set("soil3", soil3);

  Firebase.RTDB.setJSON(&fbdo, "/greenhouse/current", &json);

  // ================= FAN CONTROL =================
  if (Firebase.RTDB.getString(&fbdo, "/greenhouse/fan_status")) {

    String cmd = fbdo.stringData();
    cmd.trim();

    if (cmd == "ON") fanOn();
    else if (cmd == "OFF") fanOff();
  }

  // ================= PUMP 1 =================
  if (Firebase.RTDB.getString(&fbdo, "/greenhouse/pump1")) {

    String cmd = fbdo.stringData();
    cmd.trim();

    if (cmd == "ON") pumpOn(PUMP1_PIN, "PUMP1");
    else pumpOff(PUMP1_PIN, "PUMP1");
  }

  // ================= PUMP 2 =================
  if (Firebase.RTDB.getString(&fbdo, "/greenhouse/pump2")) {

    String cmd = fbdo.stringData();
    cmd.trim();

    if (cmd == "ON") pumpOn(PUMP2_PIN, "PUMP2");
    else pumpOff(PUMP2_PIN, "PUMP2");
  }

  // ================= PUMP 3 =================
  if (Firebase.RTDB.getString(&fbdo, "/greenhouse/pump3")) {

    String cmd = fbdo.stringData();
    cmd.trim();

    if (cmd == "ON") pumpOn(PUMP3_PIN, "PUMP3");
    else pumpOff(PUMP3_PIN, "PUMP3");
  }
}