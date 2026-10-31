/*
 * Multi-Sensor Monitoring & Alert System (Fixed for Proteus Wiring)
 */

// Pin Definitions matched to your circuit
const int MQ2_PIN        = A0;  // Gas Sensor OUT -> Arduino A0
const int PIR_PIN        = 2;   // PIR Sensor OUT -> Arduino Pin 2
const int TRIG_PIN       = 8;   // Ultrasonic Trigger -> Arduino Pin 8
const int ECHO_PIN       = 9;   // Ultrasonic Echo    -> Arduino Pin 9
const int BUZZER_PIN     = 10;  // Buzzer -> Arduino Pin 10
const int LED_PIN        = 11;  // Red LED -> Arduino Pin 11

const int DISTANCE_THRESHOLD_CM = 30; // Alert if obstacle closer than 30cm

void setup() {
  Serial.begin(9600);

  // Setup pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(MQ2_PIN, INPUT); // A0 can be used as a digital input pin

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Start with alarm OFF
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(TRIG_PIN, LOW);

  Serial.println("==========================================");
  Serial.println(" System Ready - Monitoring Started ");
  Serial.println("==========================================");
}

void loop() {
  // 1. Read Gas Sensor (Digital read for Proteus TEP Library)
  int gasDetected = digitalRead(MQ2_PIN);

  // 2. Read PIR Sensor
  int motionDetected = digitalRead(PIR_PIN);

  // 3. Read Ultrasonic Sensor
  long distanceCm = measureDistance();

  // 4. Print results to Virtual Terminal
  Serial.print("Gas: ");
  Serial.print(gasDetected == HIGH ? "DETECTED! " : "CLEAN     ");
  
  Serial.print("| Motion: ");
  Serial.print(motionDetected == HIGH ? "DETECTED! " : "NONE      ");

  Serial.print("| Distance: ");
  if (distanceCm > 0) {
    Serial.print(distanceCm);
    Serial.print(" cm  ");
  } else {
    Serial.print("Scanning... ");
  }

  // 5. Alarm Trigger Conditions
  bool gasAlert      = (gasDetected == HIGH);
  bool intruderAlert = (motionDetected == HIGH);
  bool obstacleAlert = (distanceCm > 0 && distanceCm < DISTANCE_THRESHOLD_CM);

  if (gasAlert || intruderAlert || obstacleAlert) {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    
    Serial.print("--> [ALARM ON: ");
    if (gasAlert)      Serial.print("GAS LEAK! ");
    if (intruderAlert) Serial.print("INTRUDER! ");
    if (obstacleAlert) Serial.print("NEAR OBJECT! ");
    Serial.println("]");
  } else {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("--> [NORMAL]");
  }

  delay(400);
}

long measureDistance() {
  // Trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure Echo pulse (default 1-second timeout for Proteus timing)
  long duration = pulseIn(ECHO_PIN, HIGH);

  if (duration <= 0) {
    return -1;
  }

  // Calculate distance in cm
  return (duration * 0.0343) / 2;
}