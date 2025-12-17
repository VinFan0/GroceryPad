/*
* BLE.cpp
* Written by Ryan Beck
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define LED_PIN 8   // Active LOW

#define SERVICE_UUID           "4fafc201-1fb5-459e-8fcc-c5c9c331914b" // Grocery Pad Service
#define CHAR_VALUE1_UUID       "beb5483e-36e1-4688-b7f5-ea07361b26a8" // List Name
#define CHAR_VALUE2_UUID       "cba1d466-344c-4be3-ab3f-189f80dd7518" // Item Count
#define CHAR_VALUE3_UUID       "1faff866-8efb-4dab-8cfe-c179619035ad" // Item Name

/* ────────────────────────────────────────────── */
/* Server connection callbacks                   */
/* ────────────────────────────────────────────── */
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("BLE CONNECTED");
    digitalWrite(LED_PIN, LOW);   // LED ON (active low)
  }

  void onDisconnect(BLEServer* pServer) {
    Serial.println("BLE DISCONNECTED");
    digitalWrite(LED_PIN, HIGH);  // LED OFF

    delay(100);  // Required for stability
    pServer->getAdvertising()->start();
    Serial.println("BLE advertising restarted");
  }
};


/* ────────────────────────────────────────────── */
/* Characteristic 1 callback                      */
/* List Name                                      */
/* ────────────────────────────────────────────── */
class Value1Callbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();
    Serial.print("List Name: ");
    Serial.print(value);
  }
};

/* ────────────────────────────────────────────── */
/* Characteristic 2 callback                      */
/* Item Count                                     */
/* ────────────────────────────────────────────── */
class Value2Callbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {

    uint8_t* data = pCharacteristic->getData();
    size_t len = pCharacteristic->getLength();

    if (len != sizeof(uint32_t)) {
      Serial.print("Item count write error, length = ");
      Serial.println(len);
      return;
    }

    uint32_t itemCount;
    memcpy(&itemCount, data, sizeof(uint32_t));

    Serial.print("Item count: ");
    Serial.println(itemCount);
  }
};


/* ────────────────────────────────────────────── */
/* Characteristic 3 callback                      */
/* Item Name                                      */
/* ────────────────────────────────────────────── */
class Value3Callbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();
    Serial.print("Item to add: ");
    Serial.print(value);
  }
};

/* EXAMPLE SETUP
void setup() {

  BLEDevice::init("GroceryPad");	// Name BLE device

  BLEServer *pServer = BLEDevice::createServer();	// Instantiate BLE Server
  pServer->setCallbacks(new MyServerCallbacks());	// Initiate Server callback

  BLEService *pService = pServer->createService(SERVICE_UUID); // Instantiate GroceryPad Service

  // ───── Characteristic 1 ─────
  // 		 List Name 
  BLECharacteristic *value1Char =
    pService->createCharacteristic(
      CHAR_VALUE1_UUID,
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_WRITE
    );

  value1Char->setCallbacks(new Value1Callbacks());	// Initiate List Name callback
  value1Char->setValue("No List");	// Initial List Name -> No List

  // ───── Characteristic 2 ─────
  // ──────── Item Count ────────
  BLECharacteristic *value2Char =
    pService->createCharacteristic(
      CHAR_VALUE2_UUID,
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_WRITE
    );

  value2Char->setCallbacks(new Value2Callbacks()); // Initiate Item Count callback
  uint32_t initialCount = 0;
  value2Char->setValue((uint8_t*)&initialCount, sizeof(initialCount));	// Initial Item Count -> 0

  // ───── Characteristic 3 ─────
  // ─────── Item Details ───────
  BLECharacteristic *value3Char =
    pService->createCharacteristic(
      CHAR_VALUE3_UUID,
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_WRITE
    );

  value3Char->setCallbacks(new Value3Callbacks());	// Initiate Item Name callback
  value3Char->setValue("No Item");	// Initial Item Name -> No Item

  pService->start();	// Start GroceryPad Service

  BLEAdvertising *pAdvertising = pServer->getAdvertising();	// Initiate Server Advertising
  pAdvertising->start();	// Start Advertising

  Serial.println("BLE advertising started");
}
*/