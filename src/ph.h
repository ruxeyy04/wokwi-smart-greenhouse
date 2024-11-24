#include <Arduino.h>

float calibration_value = 18.20 - 0.7;
int phval = 0;
unsigned long int avgval;
int buffer_arr[10], temporary;
float ph_act;

namespace pin {
const byte ph_sensor = 35;
}



void ph_Sensor() {
  // Collect pH sensor data
  for (int i = 0; i < 10; i++) {
    buffer_arr[i] = analogRead(pin::ph_sensor);
    delay(30);
  }

  // Sort sensor readings
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buffer_arr[i] > buffer_arr[j]) {
        temporary = buffer_arr[i];
        buffer_arr[j] = temporary;
        buffer_arr[i] = buffer_arr[j];
      }
    }
  }

  avgval = 0;
  for (int i = 2; i < 8; i++) {
    avgval += buffer_arr[i];
  }
  float volt = (float)avgval * 3.3 / 4096.0 / 6;
  ph_act = -5.7 * volt + calibration_value;
  Serial.print("pH Value: ");
  Serial.println(ph_act);


}
