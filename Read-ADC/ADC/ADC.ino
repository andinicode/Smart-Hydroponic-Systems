#define TDS_PIN 34
#define VREF 3.3
#define ADC_RESOLUTION 4095.0

unsigned long lastReadTime = 0;
const unsigned long interval = 1000;

int count = 0;
float adcSum = 0;
float voltSum = 0;

// Untuk median average filter
int buffer_arr[10], temp;

float getFilteredADC(int pin) {
  for (int i = 0; i < 10; i++) {
    buffer_arr[i] = analogRead(pin);
    delay(10);
  }

  // Sorting
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buffer_arr[i] > buffer_arr[j]) {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }

  // Ambil 6 data tengah
  int sum = 0;
  for (int i = 2; i < 8; i++) {
    sum += buffer_arr[i];
  }

  return sum / 6.0;
}

void setup() {
   analogSetWidth(12);         
  analogSetAttenuation(ADC_11db); 

  Serial.begin(115200);
  Serial.println("Kalibrasi TDS - Median + Average");
  Serial.println("Data tiap detik, rata-rata 1 menit");
  Serial.println("--------------------------------");
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastReadTime >= interval) {
    lastReadTime = currentTime;

    float adcValue = getFilteredADC(TDS_PIN);
    float voltage = (adcValue / ADC_RESOLUTION) * VREF;

    adcSum += adcValue;
    voltSum += voltage;
    count++;

    // === TAMPILAN TIAP DETIK ===
    Serial.print("Detik ");
    Serial.print(count);
    Serial.print(" -> ADC: ");
    Serial.print(adcValue, 2);
    Serial.print(" | Volt: ");
    Serial.print(voltage, 3);
    Serial.println(" V");
  }

  // === RATA-RATA 1 MENIT ===
  if (count >= 60) {
    float avgADC = adcSum / 60.0;
    float avgVolt = voltSum / 60.0;

    Serial.println("--------------------------------");
    Serial.print("RATA-RATA 1 MENIT -> ADC: ");
    Serial.print(avgADC, 2);
    Serial.print(" | Tegangan: ");
    Serial.print(avgVolt, 3);
    Serial.println(" V");
    Serial.println("--------------------------------");

    // Reset
    count = 0;
    adcSum = 0;
    voltSum = 0;
  }
}