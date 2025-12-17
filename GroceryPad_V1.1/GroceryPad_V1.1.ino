/*
*   Project: Grocery List Display
*   Author: Ryan Beck
*
*   Board: ESP32-C3 SuperMini
*/

#include "epaper.h"
#include "ble_device.h"

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(LED_BUILTIN, OUTPUT);

  epaperInit();
  bleInit();
}

void loop() {
  if (bleConnected) {
    // BLE Connected
    digitalWrite(LED_PIN, HIGH); // Solid ON

    if (updatePending && readyToUpdate) {
      Serial.println("Received display update");
      updatePending = false;
      readyToUpdate = false;
      //drawList(syncedList);
    }
  } else {
    // BLE Advertising
    digitalWrite(LED_PIN, LOW); // Short OFF
    delay(300);
    digitalWrite(LED_PIN, HIGH); // Long ON
    delay(900);
  }
}
