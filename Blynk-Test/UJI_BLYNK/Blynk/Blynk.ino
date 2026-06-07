#define BLYNK_TEMPLATE_ID "TMPL6aFCdmBzY"
#define BLYNK_TEMPLATE_NAME "Hidroponik Farm"
#define BLYNK_AUTH_TOKEN "hKXaPSGHIwd4h-6b2muaMU3sC91ArMhi"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "mariberkoneksi";
char pass[] = "HITUNG AJA";

#define TDS_PIN 34

#define VPIN_TDS V1

float tdsK = 1.7;

unsigned long lastMillis = 0;

void setup() {
  Serial.begin(115200);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  pinMode(TDS_PIN, INPUT);
}

void loop() {
  Blynk.run();

  if (millis() - lastMillis >= 10000) {
    lastMillis = millis();
    readTDS();
  }
}

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

  Blynk.virtualWrite(VPIN_TDS, tdsInt);

  Serial.print("TDS = ");
  Serial.print(tdsInt);
  Serial.println(" ppm");
}
