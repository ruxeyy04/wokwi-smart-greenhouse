#include <Adafruit_SSD1306.h>
#include <WiFi.h> // Include WiFi library for ESP32/ESP8266

extern Adafruit_SSD1306 display;

void wifiSetup() {
  const char* ssid = "Wokwi-GUEST";
  const char* password = "";
  const int channel = 6;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  
  display.println("Connecting to WiFi...");
  display.display();

  WiFi.begin(ssid, password, channel);

  unsigned long startTime = millis();
  while (millis() - startTime < 10000) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("WiFi connected!");
      
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("WiFi Connected!");
      display.println("IP Address:");
      display.println(WiFi.localIP());
      display.display();

      return; // Exit the function once connected
    }
    delay(1000); // Wait 1 second before checking again
    Serial.print(".");
  }

  // If connection failed
  Serial.println("WiFi connection failed!");
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi Connection Failed!");
  display.display();
}
