// ============================
// BLYNK SETTINGS
// ============================
#define BLYNK_TEMPLATE_ID "TMPL6aFCdmBzY"
#define BLYNK_TEMPLATE_NAME "Hidroponik Farm"
#define BLYNK_AUTH_TOKEN "hKXaPSGHIwd4h-6b2muaMU3sC91ArMhi"

// ============================
// LIBRARY
// ============================
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// ============================
// WIFI
// ============================
char ssid[] = "mariberkoneksi";
char pass[] = "HITUNG AJA";

// ============================
// PIN SETUP
// ============================
// TDS sensor output -> ESP32 GPIO 34
#define TDS_PIN 34

// Virtual pin untuk menampilkan TDS di Blynk
#define VPIN_TDS V1

// Faktor kalibrasi
float tdsK = 1.7;

// Waktu
unsigned long lastMillis = 0;

// ============================
// SETUP
// ============================
void setup() {
  Serial.begin(115200);

  // Koneksi ke Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Pin analog
  pinMode(TDS_PIN, INPUT);
}

// ============================
// LOOP
// ============================
void loop() {
  Blynk.run();

  // Baca setiap 1 detik
  if (millis() - lastMillis >= 10000) {
    lastMillis = millis();
    readTDS();
  }
}

// ============================
// FUNGSI BACA TDS
// ============================
void readTDS() {
  long adcSum = 0;
  for (int i = 0; i < 10; i++) {
    adcSum += analogRead(TDS_PIN);
    delay(5);
  }

  int adc = adcSum / 10;
  float voltage = adc * (3.3 / 4095.0);

  float tdsRaw = (133.42 * voltage * voltage * voltage
                  - 255.86 * voltage * voltage
                  + 857.39 * voltage) * 0.5;

  tdsRaw *= tdsK;

  int tdsInt = (int)(tdsRaw + 0.5);

  // Kirim ke Blynk
  Blynk.virtualWrite(VPIN_TDS, tdsInt);

  Serial.print("TDS = ");
  Serial.print(tdsInt);
  Serial.println(" ppm");
}
