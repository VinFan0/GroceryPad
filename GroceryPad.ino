/*
*   Project: Grocery List Display
*   Author: Ryan Beck
*   Description: E-Paper display driver with HTTP server hosting 
*   for grocery list collection. When a POST is recieved by the
*   server, the data is parsed and writted to the E-Paper display.
*/

/*
* E-Paper Module
*/
// base class GxEPD2_GFX can be used to pass references or pointers to the display instance as parameter, uses ~1.2k more code
// enable or disable GxEPD2_GFX base class
#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>

// ESP32 CS(SS)=5,SCL(SCK)=18,SDA(MOSI)=23,BUSY=4,RES(RST)=19,DC=15
#define CS_PIN (5)
#define BUSY_PIN (4)
#define RES_PIN (19)
#define DC_PIN (15)

// 3.7'' EPD Module
GxEPD2_BW<GxEPD2_370_GDEY037T03, GxEPD2_370_GDEY037T03::HEIGHT> display(GxEPD2_370_GDEY037T03(/*CS=5*/ CS_PIN, /*DC=*/ DC_PIN, /*RES=*/ RES_PIN, /*BUSY=*/ BUSY_PIN)); // GDEY037T03 240x416, UC8253

#define START_LINE 25
#define LINE_OFFSET 25
#define START_GROCERY 45
#define GROCERY_X 15

/*
* HTTP server Module
*/
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "B-Fiber 2.4";
const char* password = "WEST=COOK=MINE";

WebServer server(80);


void handleGroceries() {
  // Receive POST data
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Missing body");
    return;
  }

  String body = server.arg("plain");
  Serial.println("Raw text received:");
  Serial.println(body);

  // Convert to C-style array of const char*
  static const int MAX_GROCERIES = 50;
  static const char* groceries[MAX_GROCERIES];
  static char buffer[2048]; // adjust as needed

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

  server.send(200, "text/plain", "Groceries received OK");

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
    for(int i=0; i<count; i++) {
      display.setCursor(GROCERY_X, i*LINE_OFFSET + START_GROCERY);
      display.print(groceries[i]);
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

  // E-Paper Display
  display.init(115200,true,50,false);
  display.setRotation(2);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();

  // Draw page outline
  drawGroceryList();

  // HTTP Server
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.print("IP: "); Serial.println(WiFi.localIP());

  server.on("/reminder", HTTP_POST, handleGroceries);
  server.begin();
  Serial.println("HTTP server ready");
}

void loop() {
  // Server handling
  server.handleClient();

  // Writing to the display happens in the POST handler to
  // reduce required screen refreshes
}
