#define TDS_PIN 34
#define VREF 3.3
#define ADC_RESOLUTION 4095.0

int buffer_arr[10], temp;
unsigned long int avgval;

float tdsSum = 0;
int count = 0;
unsigned long lastReadTime = 0;
const unsigned long interval = 1000; // 1 detik

float getFilteredADC(int pin) {
  for (int i = 0; i < 10; i++) {
    buffer_arr[i] = analogRead(pin);
    delay(30);
  }

  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buffer_arr[i] > buffer_arr[j]) {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }

  avgval = 0;
  for (int i = 2; i < 8; i++) {
    avgval += buffer_arr[i];
  }

  return avgval / 6.0;
}

// Kalibrasi TDS (polinomial orde 2)
float calibrateTDS(float adc) {
  float tds= 0.0001 * adc * adc + 0.1863 * adc - 1e-12;
//  float tds = tds_raw *0.94;
  // Batasi nilai ppm antara 1 dan 1000
  if (tds < 0) {
    tds = 0;
  // } else if (tds > 1000.0) {
  //   tds = 1000.0;
  // }
  }
  return tds;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Kalibrasi TDS - Realtime & Rata-rata");
  Serial.println("Serial Monitor Only");
  Serial.println("-----------------------------------");
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastReadTime >= interval) {
    lastReadTime = currentTime;

    float filteredADC = getFilteredADC(TDS_PIN);
    float tds = calibrateTDS(filteredADC);

    // === REALTIME TIAP DETIK ===
    Serial.print("Detik ");
    Serial.print(count + 1);
    Serial.print(" | TDS: ");
    Serial.print(tds, 2);
    Serial.println(" ppm");

    // Akumulasi untuk rata-rata
    tdsSum += tds;
    count++;
  }

  // === RATA-RATA 1 MENIT (60 DETIK) ===
  if (count >= 60) {
    float avgTDS = tdsSum / 60.0;

    Serial.println("-----------------------------------");
    Serial.print("Rata-rata TDS (1 Menit): ");
    Serial.print(avgTDS, 2);
    Serial.println(" ppm");
    Serial.println("-----------------------------------");

    // Reset
    tdsSum = 0;
    count = 0;
  }
}