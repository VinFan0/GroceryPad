#ifndef __EPAPER_H_
#define __EPAPER_H_
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
#define START_ITEM 45
#define ITEM_X 18

struct t_listItem{
  String itemName;
  bool checked;
};

struct t_displayList{
  String listName = "";
  int itemCount = 0;
  std::vector<t_listItem> listItems;
};

t_displayList *syncedList = new t_displayList();

void drawList(t_displayList *list) {
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);

    // Page Headers Lines
    display.fillRect(0, 22, 240, 3, GxEPD_BLACK); // Bold line
    display.drawLine(10, 0, 10, 416, GxEPD_BLACK); // Vertical line
    
    // Page Lines
    for (int i=0; i<16; i++) {
      display.drawLine(0, START_LINE+LINE_OFFSET*i, 240, START_LINE+LINE_OFFSET*i, GxEPD_BLACK);
    }

    // List Name
    display.setCursor(ITEM_X, 15);
    if (list->listName != "") {
      display.print(list->listName);
    } else {
      display.print("No List");
    }

    // List Items
    display.setCursor(ITEM_X, START_ITEM);
    if (list->itemCount != 0) {
      int displayCount = 0;
      if (list->itemCount > 15) {
        displayCount = 15;
      } else {
        displayCount = list->itemCount;
      }
      for (int i=0; i<displayCount; i++) {
        display.setCursor(ITEM_X, START_ITEM + i*LINE_OFFSET);
        display.print(list->listItems[i].itemName);
        if(list->listItems[i].checked) {
          display.fillRect(12, 40 + i*LINE_OFFSET , 15*list->listItems[i].itemName.length(), 2, GxEPD_BLACK);
        }
      }
    } else {
      display.print("No items");
    }
  }
  while (display.nextPage());
}

void clearSyncedList(t_displayList *list) {
  list->listName = "";
  list->itemCount = 0;
  list->listItems.clear();
}

void epaperInit() {
  // E-Paper Display
  display.init(115200,true,50,false);
  display.setRotation(2);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();

  // Draw page outline
  drawList(syncedList);
  Serial.println("Display ready");  
}
#endif //__EPAPER_H_