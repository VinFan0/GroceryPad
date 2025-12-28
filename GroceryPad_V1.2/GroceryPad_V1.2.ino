/*
*   Project: Grocery List Display
*   Author: Ryan Beck
*
*   Board: ESP32-C3 SuperMini
*/

#include "epaper.h"
#include "ble_device.h"

void printList(t_displayList* list) {
  if (list->listName != "") {
    Serial.print(list->listName);
    Serial.print(" (");
    Serial.print(list->itemCount);
    Serial.println(")");
    Serial.println("*****************");
    for(int i=0; i<list->itemCount; i++) {
      if (list->listItems[i].checked) {
        Serial.print("X ");
      } else {
        Serial.print("  ");
      }
      Serial.println(list->listItems[i].itemName);
    }
  } else {
    Serial.println("Blank list");
  }
}

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
      printList(syncedList);
      drawList(syncedList);
    }
  } else {
    // BLE Advertising
    digitalWrite(LED_PIN, LOW); // Short OFF
    delay(300);
    digitalWrite(LED_PIN, HIGH); // Long ON
    delay(900);

    if (!clearedOnDisconnect) {
      clearSyncedList(syncedList);
      updatePending = false;
      readyToUpdate = false;
      clearedOnDisconnect = true;
      printList(syncedList);
      drawList(syncedList);
    }
  }
}
