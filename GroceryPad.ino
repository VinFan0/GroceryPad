/*
*   Project: Grocery List Display
*   Author: Ryan Beck
*   Description: E-Paper display driver with HTTP server hosting 
*   for grocery list collection. When a POST is recieved by the
*   server, the data is parsed and writted to the E-Paper display.
*
*   Board: ESP32-C3 SuperMini
*/

/*
* E-Paper Module
*/
// base class GxEPD2_GFX can be used to pass references or pointers to the display instance as parameter, uses ~1.2k more code
// enable or disable GxEPD2_GFX base class
#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <WiFi.h>
#include <WebServer.h>
#include "ssid_options.h"

/* ESP32-C3 SuperMini
        CS(SS)=7
        SCL(SCK)=4
        SDA(MOSI)=6
        BUSY=20
        RES(RST)=1
        DC=21
*/
#define CS_PIN (7)
#define BUSY_PIN (20)
#define RES_PIN (1)
#define DC_PIN (21)

// 3.7'' EPD Module
GxEPD2_BW<GxEPD2_370_GDEY037T03, GxEPD2_370_GDEY037T03::HEIGHT> display(GxEPD2_370_GDEY037T03(
                                                                            /*CS=7*/ CS_PIN, 
                                                                            /*DC=21*/ DC_PIN, 
                                                                            /*RES=1*/ RES_PIN, 
                                                                            /*BUSY=20*/ BUSY_PIN)); // GDEY037T03 240x416, UC8253

#define START_LINE 25
#define LINE_OFFSET 25
#define START_GROCERY 45
#define GROCERY_X 15

// /*
// * HTTP server Module
// */
WebServer server(80);

void handleGroceries() {
  // Receive POST data
  // if (!server.hasArg("plain")) {
  //   server.send(400, "text/plain", "Missing body");
  //   return;
  // }

  String body = server.arg("plain");
  Serial.println("Raw text received:");
  Serial.println(body);

  // Convert to C-style array of const char*
  static const int MAX_GROCERIES = 50;
  static const char* groceries[MAX_GROCERIES];
  static char buffer[2048]; // adjust as needed
  char resized_grocery[18];

  // Copy body into a writable buffer
  strncpy(buffer, body.c_str(), sizeof(buffer));
  buffer[sizeof(buffer) - 1] = '\0'; // ensure null-termination

  // Split by line breaks or commas
  int count = 0;
  char* token = strtok(buffer, "\r\n,");
  while (token != nullptr && count < MAX_GROCERIES) {
    groceries[count++] = token;
    token = strtok(nullptr, "\r\n,");
  }

  // Debug output
  Serial.printf("Parsed %d groceries:\n", count);
  for (int i = 0; i < count; i++) {
    Serial.printf("  [%d] %s\n", i, groceries[i]);
  }

  server.send(200, "text/plain", "Groceries received OK\n");

  // Update E-Paper display
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);

    display.setCursor(GROCERY_X, 15);
    display.print("Grocery List");
    display.fillRect(0, 22, 240, 3, GxEPD_BLACK); // Bold line
    display.drawLine(10, 0, 10, 416, GxEPD_BLACK); // Vertical line

    for (int i=0; i<16; i++) {
      display.drawLine(0, START_LINE+LINE_OFFSET*i, 240, START_LINE+LINE_OFFSET*i, GxEPD_BLACK);
    }

    if (count == 0) {
      display.setCursor(GROCERY_X, START_GROCERY);
      display.print("No groceries");
    }
    if (count <= 15) {
      for(int i=0; i<count; i++) {
        display.setCursor(GROCERY_X, i*LINE_OFFSET + START_GROCERY);
        if(strlen(groceries[i]) > 20) {
           strncpy(resized_grocery, groceries[i], 17);
           resized_grocery[17] = '\0';
           display.print(resized_grocery);
           display.print("...");
        } else display.print(groceries[i]);
      }
    } else {
      for(int i=0; i<14; i++) {
        display.setCursor(GROCERY_X, i*LINE_OFFSET + START_GROCERY);
        if(strlen(groceries[i]) > 20) {
           strncpy(resized_grocery, groceries[i], 17);
           resized_grocery[17] = '\0';
           display.print(resized_grocery);
           display.print("...");
        } else display.print(groceries[i]);
      }
      display.setCursor(GROCERY_X, 14*LINE_OFFSET+START_GROCERY);
      display.print("And ");
      display.print(count-14);
      display.print(" more");
    }
  }
  while (display.nextPage());
}

void drawGroceryList() {
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);

    display.setCursor(GROCERY_X, 15);
    display.print("Grocery List");
    display.fillRect(0, 22, 240, 3, GxEPD_BLACK); // Bold line
    display.drawLine(10, 0, 10, 416, GxEPD_BLACK); // Vertical line

    for (int i=0; i<16; i++) {
      display.drawLine(0, START_LINE+LINE_OFFSET*i, 240, START_LINE+LINE_OFFSET*i, GxEPD_BLACK);
    }

    display.setCursor(GROCERY_X, START_GROCERY);
    display.print("No groceries");
  }
  while (display.nextPage());
}

void setup() {
  Serial.begin(115200);
  delay(500);
  // while(!Serial) {;}

  pinMode(LED_BUILTIN, OUTPUT);

  // HTTP Server
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("WiFi Connected");
  Serial.print("IP: "); Serial.println(WiFi.localIP());
  server.on("/reminder", HTTP_POST, handleGroceries);
  server.begin();
  Serial.println("HTTP server ready");
  delay(200);

  // E-Paper Display
  display.init(115200,true,50,false);
  display.setRotation(2);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();

  // Draw page outline
  drawGroceryList();
  Serial.print("Display ready");  
}

void loop() {
  // Server handling
  server.handleClient();

  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);

  // Writing to the display happens in the POST handler to
  // reduce required screen refreshes
}
