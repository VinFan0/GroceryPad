/*
*   Project: Grocery List Display
*   Author: Ryan Beck
*
*   Board: ESP32-C3 SuperMini
*/

#include "BLE.cpp"
#include "EPaper.cpp"

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(LED_BUILTIN, OUTPUT);

  
}

void loop() {
  // Server handling
  server.handleClient();

  if (WiFi.status() != WL_CONNECTED) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt >= RECONNECT_INTERVAL) {
      lastReconnectAttempt = now;
      delay(500);
      Serial.println("WiFi disconnected — attempting reconnect...");
      serverInit();
    }
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }

  // Writing to the display happens in the POST handler to
  // reduce required screen refreshes
}
