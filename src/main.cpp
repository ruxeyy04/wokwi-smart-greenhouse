#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "wifiSetup.h"
#include "welcome.h"
#include <DHT.h>
#include "do.h"
#include "ph.h"
// Pins
#define DHT_PIN 14
#define LDR_PIN 32
#define WATER_LEVEL_TRIG 27
#define WATER_LEVEL_ECHO 13

#define RELAY1_PIN 5  // Misting
#define RELAY2_PIN 18 // Fan
#define RELAY3_PIN 19 // Pump
#define RELAY4_PIN 17 // Water Valve

// OLED dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// DHT Sensor
#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);

// Variables
float temperature, humidity, lightLevel, waterDistance, waterPercent;
int pumpThreshold, lightThreshold;

// Helper function to measure water level
float measureWaterDistance()
{
  digitalWrite(WATER_LEVEL_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(WATER_LEVEL_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(WATER_LEVEL_TRIG, LOW);

  long duration = pulseIn(WATER_LEVEL_ECHO, HIGH);
  return duration * 0.034 / 2; // Convert to cm
}

// Setup
void setup()
{
    // Initialize relays
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);

  // Initialize LDR, ultrasonic, and potentiometers
  pinMode(LDR_PIN, INPUT);
  pinMode(WATER_LEVEL_TRIG, OUTPUT);
  pinMode(WATER_LEVEL_ECHO, INPUT);

  // Default relay states
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);
  digitalWrite(RELAY3_PIN, LOW);
  digitalWrite(RELAY4_PIN, LOW);
  Serial.begin(115200);

  welcome();
  wifiSetup();
  // Initialize DHT
  dht.begin();



  Serial.println("Setup complete");
}

// Loop
void loop()
{
  // Read sensors
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  lightLevel = analogRead(LDR_PIN) / 40.95; // Normalize to 0-100
  waterDistance = measureWaterDistance();

  // Convert water level to percentage and constrain to 0-100%
  waterPercent = map(waterDistance, 2, 200, 100, 0); // Assuming 50 cm max depth
  waterPercent = constrain(waterPercent, 0, 100);

  // Control relays based on thresholds
  bool relay1State = false; // Misting
  bool relay2State = false; // Fan
  bool relay3State = false; // Pump
  bool relay4State = false; // Water Valve

  // Fan and misting control
  if (humidity > 90 || (temperature > 31 && humidity > 80))
  {
    digitalWrite(RELAY2_PIN, HIGH); // Activate fan
    relay2State = true;
  }
  else if (humidity < 80 && temperature < 27)
  {
    digitalWrite(RELAY2_PIN, LOW);
    relay2State = false;
  }

  if (temperature > 31)
  {
    digitalWrite(RELAY1_PIN, HIGH); // Activate misting
    relay1State = true;
  }
  else if (temperature < 27)
  {
    digitalWrite(RELAY1_PIN, LOW);
    relay1State = false;
  }

  // Water valve control
  if (waterPercent <= 40)
  {
    digitalWrite(RELAY4_PIN, HIGH); // Activate water valve
    relay4State = true;
  }
  else
  {
    digitalWrite(RELAY4_PIN, LOW);
    relay4State = false;
  }

  // Pump control
  if (waterPercent >= 100)
  {
    digitalWrite(RELAY3_PIN, HIGH); // Activate pump
    relay3State = true;
  }
  else if (waterPercent < 40)
  {
    digitalWrite(RELAY3_PIN, LOW);
    relay3State = false;
  }

  // Display data on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  ph_Sensor();
  readDOSensor();
  display.setCursor(0, 0);
  display.print("Temp: ");
  display.print(temperature);
  display.println(" C");

  display.print("Humidity: ");
  display.print(humidity);
  display.println(" %");

  display.print("Light: ");
  display.print(lightLevel);
  display.println(" %");

  display.print("Water Level: ");
  display.print(waterPercent);
  display.println(" %");

  display.print("DO: ");
  display.print(doValue, 1); // Display with 1 decimal place
  display.println(" mg/L");

  display.print("pH Level: ");
  display.print(ph_act, 1); // Display with 1 decimal place
  display.println(" pH");

  display.display();

  // Debugging data
  Serial.println("---");
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.println(" C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("Light: ");
  Serial.print(lightLevel);
  Serial.println(" %");
  Serial.print("Water Level: ");
  Serial.print(waterPercent);
  Serial.println(" %");

  // Print relay states
  Serial.print("Misting: ");
  Serial.println(relay1State ? "ON" : "OFF");
  Serial.print("Fan: ");
  Serial.println(relay2State ? "ON" : "OFF");
  Serial.print("Pump: ");
  Serial.println(relay3State ? "ON" : "OFF");
  Serial.print("Water Valve: ");
  Serial.println(relay4State ? "ON" : "OFF");
  Serial.println("---");

  // Delay for stability
  delay(2000);
}
