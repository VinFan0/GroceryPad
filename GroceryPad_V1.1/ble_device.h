#ifndef __BLE_DEVICE_H_
#define __BLE_DEVICE_H_
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
#define CHAR_VALUE4_UUID       "bdfb171f-5d4f-486d-8a11-4dae890324de" // Transmission End

bool bleConnected = false;
bool updatePending = false;
bool readyToUpdate = false;
bool clearedOnDisconnect = true;

/* ────────────────────────────────────────────── */
/* Server connection callbacks                   */
/* ────────────────────────────────────────────── */
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("BLE CONNECTED");
    //digitalWrite(LED_PIN, LOW);   // LED ON (active low)
    bleConnected = true;
    updatePending = false;
    readyToUpdate = false;
  }

  void onDisconnect(BLEServer* pServer) {
    Serial.println("BLE DISCONNECTED");
    //digitalWrite(LED_PIN, HIGH);  // LED OFF
    bleConnected = false;
    updatePending = true;
    readyToUpdate = true;
    clearedOnDisconnect = false;

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
    value.trim();
    Serial.print("Received List Name: ");
    Serial.println(value);

    syncedList->listName = value;
  }
};

/* ────────────────────────────────────────────── */
/* Characteristic 2 callback                      */
/* Item Count                                     */
/* ────────────────────────────────────────────── */
class Value2Callbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    delay(20);

    Serial.println("New list sync signal received");

    // ───── Clear your list here ─────
    clearSyncedList(syncedList);

    Serial.println("ACK sent to application through BLE management");
  }
};


/* ────────────────────────────────────────────── */
/* Characteristic 3 callback                      */
/* Item Name                                      */
/* ────────────────────────────────────────────── */
class Value3Callbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();
    value.trim(); // Remove \r \n and whitespace

    if(value.length() == 0) return;

    int spaceIndex = value.indexOf(' ');
    if (spaceIndex == -1) {
      Serial.println("Invalid Command Format");
      return;
    }

    String command = value.substring(0,spaceIndex);
    String itemName = value.substring(spaceIndex+1);

    command.toLowerCase();

    Serial.print("Command: ");
    Serial.println(command);

    Serial.print("Item: ");
    Serial.println(itemName);

    // ───── Command handling example ─────
    if (command == "add") {
      if(itemName.length() != 0) {
        t_listItem newItem;
        newItem.itemName = itemName;
        newItem.checked = false;

        syncedList->listItems.push_back(newItem);

        syncedList->itemCount = syncedList->itemCount+1;
      }
    } else if (command == "delete") {
      if (syncedList->itemCount > 0) {
        if(itemName.length() != 0) {
          for (int i=0; i<syncedList->itemCount; i++) {
            if (syncedList->listItems[i].itemName == itemName) {
              syncedList->listItems.erase(syncedList->listItems.begin() + i);

              syncedList->itemCount = syncedList->itemCount - 1;
              break;
            }
          }
        }
      }
    } else if (command == "check") {
    } else if (command == "uncheck") {
    } else {
      Serial.println("Unknown command");
    }
    
    updatePending = true;
  }
};

/* ────────────────────────────────────────────── */
/* Characteristic 4 callback                      */
/* Transmission End                               */
/* ────────────────────────────────────────────── */
class Value4Callbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    Serial.println("List Synced");
    readyToUpdate = true;
  }
};

/* EXAMPLE SETUP */
void bleInit() {

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
  // ───────── New List ─────────
  BLECharacteristic *value2Char =
  pService->createCharacteristic(
    CHAR_VALUE2_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE |
    BLECharacteristic::PROPERTY_NOTIFY
  );

  value2Char->setCallbacks(new Value2Callbacks());	// Initiate Item Name callback
  value2Char->setValue("");	// Initial Item Name -> No Item

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

  // ───── Characteristic 4 ─────
  // ───── Transmission END ─────
  BLECharacteristic *value4Char =
    pService->createCharacteristic(
      CHAR_VALUE4_UUID,
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_WRITE
    );

  value4Char->setCallbacks(new Value4Callbacks());	// Initiate Item Name callback
  value4Char->setValue("");	// Initial Item Name -> No Item

  pService->start();	// Start GroceryPad Service

  BLEAdvertising *pAdvertising = pServer->getAdvertising();	// Initiate Server Advertising
  pAdvertising->start();	// Start Advertising

  Serial.println("BLE advertising started");
}
#endif //__BLE_DEVICE_H_