#define FLOW_PIN   33
#define RELAY_PIN  22

const float PULSES_PER_LITER = 12940.0;
const float CALIBRATION_FACTOR = 3.4;  

const float TARGET_ML = 350.0;
const unsigned long TARGET_PULSE =
  (TARGET_ML / 1000.0) * PULSES_PER_LITER;

volatile unsigned long pulseCount = 0;
bool pumpOn = false;

void IRAM_ATTR onFlowPulse() {
  pulseCount++;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(FLOW_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_PIN), onFlowPulse, RISING);

  pinMode(RELAY_PIN, OUTPUT);

  pulseCount = 0;
  digitalWrite(RELAY_PIN, HIGH);
  pumpOn = true;

  Serial.println("FLOW 350 mL (SENSOR = AKTUAL)");
  Serial.print("Target pulse: ");
  Serial.println(TARGET_PULSE);
}

void loop() {
  if (pumpOn && pulseCount >= TARGET_PULSE) {
    digitalWrite(RELAY_PIN, LOW);
    pumpOn = false;

    noInterrupts();
    unsigned long pulses = pulseCount;
    interrupts();

    float mlRaw = (pulses / PULSES_PER_LITER) * 1000.0;

    float mlCalibrated = mlRaw * CALIBRATION_FACTOR;

    Serial.println("===== HASIL =====");
    Serial.printf("Pulse total       : %lu\n", pulses);
    Serial.printf("Volume (raw)      : %.2f mL\n", mlRaw);
    Serial.printf("Volume (kalibrasi): %.2f mL\n", mlCalibrated);
  }
}