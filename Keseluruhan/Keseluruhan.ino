#define BLYNK_TEMPLATE_ID "TMPL6KhksMK_t"
#define BLYNK_TEMPLATE_NAME "Selada Farm"
#define BLYNK_AUTH_TOKEN "hj8GS8X-3ZckiqeaZF6w-0XYgYdQrVLG"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "time.h"

// PIN
#define RELAY1_PIN    22
#define RELAY2_PIN    23
#define TDS_PIN       34
#define FLOW1_PIN     32
#define FLOW2_PIN     33

// WIFI 
char ssid[] = "mariberkoneksi";
char pass[] = "HITUNG AJA";

//  BLYNK VIRTUAL PINS 
#define VPIN_TDS        V0
#define VPIN_FLOW1      V3
#define VPIN_FLOW2      V4
#define VPIN_TARGET_TDS V6
#define VPIN_STATUS     V7

//  FLOW SENSOR 
const float FLOW_CALIBRATION = 3.4;
volatile unsigned long flow1Count = 0;
volatile unsigned long flow2Count = 0;
float totalFlowML1 = 0;
float totalFlowML2 = 0;

// TDS
float tdsRaw = 0;
int tdsInt = 0;
int targetTDS = 560;
String statusWaktu = "Menit 1";
int lastBlock = -1;
unsigned long lastFlowMillis = 0;
unsigned long lastTDSMillis = 0;
int tdsDisplayCount = 0;
int maxTDSDisplay = 7;
bool selesai = false;

// STATE
bool relayActive = false;
bool pumpRequest = false;

//  NTP 
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0;

// INTERRUPT 
void IRAM_ATTR onFlow1Pulse() { flow1Count++; }
void IRAM_ATTR onFlow2Pulse() { flow2Count++; }

// SETUP 
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);

  pinMode(FLOW1_PIN, INPUT_PULLUP);
  pinMode(FLOW2_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW1_PIN), onFlow1Pulse, RISING);
  attachInterrupt(digitalPinToInterrupt(FLOW2_PIN), onFlow2Pulse, RISING);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  while (time(nullptr) < 100000) delay(500);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  totalFlowML1 = totalFlowML2 = 0;
  Blynk.virtualWrite(VPIN_FLOW1, 0);
  Blynk.virtualWrite(VPIN_FLOW2, 0);
  Blynk.virtualWrite(VPIN_RELAYSTAT, 0);
  Blynk.virtualWrite(VPIN_TDS, 0);
  Blynk.virtualWrite(VPIN_STATUS, statusWaktu);
  Blynk.virtualWrite(VPIN_TARGET_TDS, targetTDS);

  Serial.println("SISTEM SIAP ");
}

//LOOP 
void loop() {
  Blynk.run();
  unsigned long nowMillis = millis();

  if(nowMillis - lastFlowMillis >= 1000){
    lastFlowMillis = nowMillis;
    readAndSendFlow();
  }

  updateAndSendStatus();

  // ===== Baca TDS tiap detik (real-time) =====
  if(nowMillis - lastTDSMillis >= 1000 && !selesai){
    lastTDSMillis = nowMillis;
    readTDS();
  }

  // ===== Kontrol pompa sesuai TDS =====
  controlRelay();
}

// ===== BACA TDS =====
void readTDS() {
  long adcSum = 0;
  for(int i=0;i<10;i++){
    adcSum += analogRead(TDS_PIN);
    delay(5);
  }

  // POLINOMIAL ORDE 2
  tdsRaw = 0.0001 * adcSum * adcSum + 0.1863 * adcSum - 1e-12;
  tdsInt = (int)(tdsRaw + 0.5);

  // Tampilkan TDS 7x per menit (menit1 = 8x)
  int intervalDisplay = (lastBlock == 0) ? 8 : 7;
  if(tdsDisplayCount < intervalDisplay){
    tdsDisplayCount++;
    Serial.printf("TDS Read %d/%d : %d ppm\n", tdsDisplayCount, intervalDisplay, tdsInt);
    Blynk.virtualWrite(VPIN_TDS, tdsInt);
  }

  if(lastBlock == 3 && tdsDisplayCount >= intervalDisplay && !selesai){
    Serial.println("=== SELESAI ===");
    selesai = true;
  }

  if(tdsInt < targetTDS) pumpRequest = true;
}

// FLOW 
void readAndSendFlow() {
  noInterrupts();
  unsigned long c1 = flow1Count;
  unsigned long c2 = flow2Count;
  flow1Count = 0; flow2Count = 0;
  interrupts();

  float v1 = c1/FLOW_CALIBRATION;
  float v2 = c2/FLOW_CALIBRATION;

  totalFlowML1 += v1;
  totalFlowML2 += v2;

  Blynk.virtualWrite(VPIN_FLOW1, totalFlowML1);
  Blynk.virtualWrite(VPIN_FLOW2, totalFlowML2);
}
void updateAndSendStatus() {
  time_t now = time(nullptr);
  struct tm *t = localtime(&now);

  int block = (t->tm_min % 4); // Menit 1-4 → 0..3
  int newTarget[] = {560,660,760,840};
  String newStatus[] = {"Menit 1","Menit 2","Menit 3","Menit 4"};

  if(block != lastBlock){
    targetTDS = newTarget[block];
    statusWaktu = newStatus[block];

    // Reset TDS display count tiap menit
    tdsDisplayCount = 0;
    selesai = false;

    // Reset flow menit 1
    if(block == 0){
      totalFlowML1 = totalFlowML2 = 0;
      Blynk.virtualWrite(VPIN_FLOW1,0);
      Blynk.virtualWrite(VPIN_FLOW2,0);
      maxTDSDisplay = 8; // menit1 = 8x
    } else {
      maxTDSDisplay = 7;
    }

    Serial.printf("=== Status: %s | Target: %d | Pompa ON ===\n", statusWaktu.c_str(), targetTDS);
    digitalWrite(RELAY1_PIN,HIGH);
    digitalWrite(RELAY2_PIN,HIGH);
    relayActive = true;

    lastBlock = block;
  }

  Blynk.virtualWrite(VPIN_TARGET_TDS,targetTDS);
  Blynk.virtualWrite(VPIN_STATUS,statusWaktu);
}
void controlRelay(){
  if(pumpRequest && !relayActive){
    digitalWrite(RELAY1_PIN,HIGH);
    digitalWrite(RELAY2_PIN,HIGH);
    relayActive = true;
    pumpRequest = false;
  }

  if(relayActive && tdsInt >= targetTDS){
    digitalWrite(RELAY1_PIN,LOW);
    digitalWrite(RELAY2_PIN,LOW);
    relayActive = false;
  }
}

