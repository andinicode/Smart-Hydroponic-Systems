#define RELAY_A_PIN  22 
#define RELAY_B_PIN  23   

#define DURASI_NYALA 3000  
#define JEDA         2000  

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(RELAY_A_PIN, OUTPUT);
  pinMode(RELAY_B_PIN, OUTPUT);

  digitalWrite(RELAY_A_PIN, LOW);
  digitalWrite(RELAY_B_PIN, LOW);

  Serial.println("TES POMPA PERISTALTIK");
  delay(1000);

  Serial.println(">> Pompa A ON");
  digitalWrite(RELAY_A_PIN, HIGH);
  delay(DURASI_NYALA);
  digitalWrite(RELAY_A_PIN, LOW);
  Serial.println(">> Pompa A OFF");
  delay(JEDA);

  Serial.println(">> Pompa B ON");
  digitalWrite(RELAY_B_PIN, HIGH);
  delay(DURASI_NYALA);
  digitalWrite(RELAY_B_PIN, LOW);
  Serial.println(">> Pompa B OFF");
  delay(JEDA);

  Serial.println(">> Pompa A + B ON (bersamaan)");
  digitalWrite(RELAY_A_PIN, HIGH);
  digitalWrite(RELAY_B_PIN, HIGH);
  delay(DURASI_NYALA);
  digitalWrite(RELAY_A_PIN, LOW);
  digitalWrite(RELAY_B_PIN, LOW);
  Serial.println(">> Pompa A + B OFF");

  Serial.println("TES SELESAI");
}

void loop() {
}