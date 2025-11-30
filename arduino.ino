#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- PIN MAPPING ---
#define SENSOR_AMBIENT_PIN 6   // Room Air
#define SENSOR_COOLER_PIN  5   // Hot Side Safety
#define RELAY_PIN          7   
#define HOT_SIDE_LIMIT     60.0 // Max temp for Peltier Hot Side

// --- LOGIC SETTINGS ---
#define TURN_ON_TEMP       60   // CPU must hit this to start cooling
// The "Turn Off" temp is dynamic (It is the Ambient Temp)

// --- DISPLAY SETUP ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- SENSORS ---
OneWire oneWireAmbient(SENSOR_AMBIENT_PIN);
DallasTemperature sensorAmbient(&oneWireAmbient);
OneWire oneWireCooler(SENSOR_COOLER_PIN);
DallasTemperature sensorCooler(&oneWireCooler);

// --- STATE VARIABLES ---
int pcCpuTemp = 0; 
bool isCooling = false; // Tracks if the relay is currently ON or OFF

void setup() {
  Serial.begin(9600); 
  
  // Relay Setup (Active LOW)
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Start OFF

  sensorAmbient.begin();
  sensorCooler.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for(;;);
  }
}

void loop() {
  // 1. GET DATA
  if (Serial.available() > 0) {
    pcCpuTemp = Serial.parseInt(); 
    while (Serial.available() > 0) Serial.read(); 
  }

  sensorAmbient.requestTemperatures();
  sensorCooler.requestTemperatures();
  float tempAmbient = sensorAmbient.getTempCByIndex(0);
  float tempCooler = sensorCooler.getTempCByIndex(0); 

  // 2. CONTROL LOGIC
  String statusMsg = "IDLE";

  // --- SAFETY CHECKS (Priority 1) ---
  if (tempCooler > HOT_SIDE_LIMIT || tempCooler == -127.0) {
    isCooling = false; // Force OFF
    statusMsg = "!HOT!";
  }
  else if (pcCpuTemp == 0) {
    isCooling = false; // Force OFF
    statusMsg = "WAIT PC";
  }
  // --- NORMAL OPERATION (Priority 2) ---
  else {
    if (isCooling) {
      // ** WE ARE CURRENTLY ON **
      // Keep running until we hit the bottom limit (Ambient)
      if (pcCpuTemp <= tempAmbient) {
        isCooling = false; // Target reached, turn OFF
      } else {
        statusMsg = "COOLING";
      }
    } 
    else {
      // ** WE ARE CURRENTLY OFF **
      // Stay off until we hit the top limit (60C)
      if (pcCpuTemp >= TURN_ON_TEMP) {
        isCooling = true; // Trigger Triggered!
      } else {
        statusMsg = "IDLE";
      }
    }
  }

  // 3. APPLY TO RELAY (Active LOW)
  if (isCooling) {
    digitalWrite(RELAY_PIN, LOW); // ON
  } else {
    digitalWrite(RELAY_PIN, HIGH); // OFF
  }

  // 4. DISPLAY
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // -- Section 1: Temps --
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Room: "); display.print(tempAmbient, 1); display.print("C");
  
  display.setCursor(0, 15);
  display.print("CPU:  "); 
  display.setTextSize(2); 
  display.print(pcCpuTemp); 
  display.setTextSize(1);
  display.print("C");

  // -- Section 2: Threshold Info --
  // Show the user what the target is
  display.setCursor(80, 0);
  display.print("Trig:");
  display.print(TURN_ON_TEMP);

  // -- Section 3: Status --
  display.drawLine(0, 35, 128, 35, SSD1306_WHITE);
  
  display.setCursor(0, 45);
  display.setTextSize(1);
  display.print("System Status:");
  
  display.setCursor(0, 55);
  display.setTextSize(2);
  display.print(statusMsg);

  display.display();
  delay(100); 
}