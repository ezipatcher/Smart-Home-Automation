#include <ESP32Servo.h>

// ==========================================
// PIN DEFINITIONS
// ==========================================
#define THERMISTOR_PIN 34    // KY-028 Analog Pin
#define LDR_PIN 35           // LDR Analog Pin
#define PIR_PIN 32           // Shared PIR Sensor
#define DOOR_BUTTON 21       // Doorbell Push Button
#define SECURITY_BUTTON 18   // Security Mode Push Button

#define FAN_LED 26           // Green LED for Fan
#define HOUSE_LED 25         // Yellow LED for House Lights
#define RED_LED 14           // Shared Red LED (Intruder/Security)
#define WELCOME_LED 15       // Blue LED for Welcome Light
#define BUZZER_PIN 27        // Common Buzzer
#define SERVO_PIN 13         // Door Servo

// ==========================================
// SYSTEM THRESHOLDS 
// ==========================================
// Tune these values based on your Wokwi environment
const int TEMP_THRESHOLD = 1800; 
const int LIGHT_THRESHOLD = 2000;

// ==========================================
// STATE VARIABLES
// ==========================================
// Button States
bool lastDoorButtonState = HIGH;
bool lastSecButtonState = HIGH;


// Security System
bool securityMode = false;
unsigned long lastFlashTime = 0;
bool flashState = false;
bool intruderBeepPlayed = false;
// Door System
Servo doorServo;
bool doorOpened = false;
unsigned long doorOpenTime = 0;
const int DOOR_OPEN_DURATION = 3000; // Door stays open for 3 seconds

// Temp System Buzzer Timer
bool tempBuzzerPlayed = false;
unsigned long tempBuzzerStartTime = 0;

void setup() {
  Serial.begin(115200);

  // --- INPUTS ---
  pinMode(THERMISTOR_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(PIR_PIN, INPUT); // Wokwi PIR defaults are usually fine with standard INPUT
  
  // Using ESP32 Internal Pull-ups for both buttons
  pinMode(DOOR_BUTTON, INPUT_PULLUP); 
  pinMode(SECURITY_BUTTON, INPUT_PULLUP); 

  // --- OUTPUTS ---
  pinMode(FAN_LED, OUTPUT);
  pinMode(HOUSE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(WELCOME_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initial States -> Everything OFF
  digitalWrite(FAN_LED, LOW);
  digitalWrite(HOUSE_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(WELCOME_LED, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // Initialize Servo
  doorServo.attach(SERVO_PIN);
  doorServo.write(0); // Ensure door starts closed
  lastDoorButtonState = digitalRead(DOOR_BUTTON);
  lastSecButtonState = digitalRead(SECURITY_BUTTON);
  Serial.println("Smart Home Capstone Project Ready.");
}

void loop() {
  unsigned long currentMillis = millis();
  
  // These variables help us figure out if the buzzer should be ON this frame
  bool requestBuzzerTemp = false;
  bool requestBuzzerSecurity = false;

  // Create a continuous blinking heartbeat every 200ms for flash effects
  if (currentMillis - lastFlashTime >= 200) {
    lastFlashTime = currentMillis;
    flashState = !flashState;
  }

  // ----------------------------------------
  // 1. THERMISTOR: TEMPERATURE & FAN
  // ----------------------------------------
  int tempValue = analogRead(THERMISTOR_PIN);
  
  // Depending on your KY-028 setup, it might trigger below or above the threshold. 
  // Adjust the `<` to `>` if it acts backward in the simulation.
  if (tempValue < TEMP_THRESHOLD) {
    digitalWrite(FAN_LED, HIGH); 
    
    if (!tempBuzzerPlayed) {
      if (tempBuzzerStartTime == 0) tempBuzzerStartTime = currentMillis;
      
      // Request buzzer for the first 2 seconds of the heat spike
      if (currentMillis - tempBuzzerStartTime < 2000) {
        requestBuzzerTemp = true; 
      } else {
        tempBuzzerPlayed = true; 
      }
    }
  } else {
    digitalWrite(FAN_LED, LOW); 
    tempBuzzerPlayed = false;
    tempBuzzerStartTime = 0;
  }

  // ----------------------------------------
  // 2. LDR: AUTOMATIC LIGHTING
  // ----------------------------------------
  int ldrValue = analogRead(LDR_PIN);
  if (ldrValue < LIGHT_THRESHOLD) {
    digitalWrite(HOUSE_LED, HIGH); // Dark -> Lights ON
  } else {
    digitalWrite(HOUSE_LED, LOW);  // Bright -> Lights OFF
  }

  // ----------------------------------------
// 3. DOORBELL BUTTON & SERVO LOGIC
// ----------------------------------------

bool currentDoorState = digitalRead(DOOR_BUTTON);

// Detect button press
if (currentDoorState == LOW && lastDoorButtonState == HIGH) {

  delay(20);   // Debounce

  if (digitalRead(DOOR_BUTTON) == LOW && !doorOpened) {

    Serial.println("Doorbell Pressed -> Opening Door");

    doorServo.write(90);               // Open door
    digitalWrite(WELCOME_LED, HIGH);   // Welcome LED ON

    doorOpened = true;
    doorOpenTime = millis();
  }
}

lastDoorButtonState = currentDoorState;

// Auto close door after 3 seconds
if (doorOpened && (millis() - doorOpenTime >= DOOR_OPEN_DURATION)) {

  doorServo.write(0);                  // Close door
  digitalWrite(WELCOME_LED, LOW);      // Welcome LED OFF

  doorOpened = false;

  Serial.println("Door Closed");
}
  // ----------------------------------------
  // 4. SECURITY MODE TOGGLE BUTTON
  // ----------------------------------------
  bool currentSecState = digitalRead(SECURITY_BUTTON);
  if (currentSecState == LOW && lastSecButtonState == HIGH) {
    delay(50); // Debounce
    securityMode = !securityMode; 
    
    Serial.print("Security Mode: ");
    Serial.println(securityMode ? "ON" : "OFF");
    
    // Quick beep to acknowledge mode change
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
  }
  lastSecButtonState = currentSecState;

  // ----------------------------------------
 // ----------------------------------------
  // ----------------------------------------
// 5. PIR SENSOR LOGIC
// ----------------------------------------

bool motionDetected = (digitalRead(PIR_PIN) == HIGH);
if (securityMode) {
  // -------- SECURITY MODE ON --------
  if (motionDetected) {
    // Flash LED
    digitalWrite(RED_LED, flashState);
    // Continuous buzzer
    requestBuzzerSecurity = true;
  }
  else {
    digitalWrite(RED_LED, LOW);
    requestBuzzerSecurity = false;
  }
}
else {
  // -------- SECURITY MODE OFF --------
  if (motionDetected) {
    // Intruder indication
    digitalWrite(RED_LED, HIGH);

    // Beep only once
    if (!intruderBeepPlayed) {

     if (!intruderBeepPlayed) {
    requestBuzzerSecurity = true;
    intruderBeepPlayed = true;
}
    }

  }
  else {

    digitalWrite(RED_LED, LOW);

    // Ready for next motion
    intruderBeepPlayed = false;
  }
}

  // This ensures the Temperature and Security systems don't fight over the buzzer
  // ----------------------------------------
// 6. MASTER BUZZER CONTROL
// ----------------------------------------

if (requestBuzzerSecurity) {
  digitalWrite(BUZZER_PIN, HIGH);
}
else if (requestBuzzerTemp) {
  digitalWrite(BUZZER_PIN, HIGH);
}
else {
  digitalWrite(BUZZER_PIN, LOW);
}
}
