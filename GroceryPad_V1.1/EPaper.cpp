/*
* EPaper.cpp
* Written by Ryan Beck
*/
// base class GxEPD2_GFX can be used to pass references or pointers to the display instance as parameter, uses ~1.2k more code
// enable or disable GxEPD2_GFX base class

#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>

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

epaperInit() {
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