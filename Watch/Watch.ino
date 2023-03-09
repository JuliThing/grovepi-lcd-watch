// Somehow written by Juli/ JuliThing (on GH)
#include <Wire.h>
#include "rgb_lcd.h"
#include <DS1307.h>
#define CLK 2
#define DT 3
#define SW 4

#define LCDWIDTH 16
#define LCDHEIGHT 2

unsigned long initialPress;         // initial start for the timer
unsigned long noPress;              // time after the button is let go
const unsigned long period = 2000;  //2 seconds in millisec form

int lastState;

rgb_lcd lcd;
DS1307 rtc;

/////
using Task = void (*)();
typedef struct MenuEntry MenuEntry;
// menu stuffs
void launchTask(Task task) {task();}

struct MenuEntry {
  char topline[LCDWIDTH + 1];
  char botline[LCDWIDTH + 1];
  Task target;
};
void renderEntry(MenuEntry* entry) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(entry -> topline, LCDWIDTH);
  lcd.setCursor(0, 1);
  lcd.write(entry -> botline, LCDWIDTH);
}

// NUM MUST BE GREATER THAN 0
Task doMenu(MenuEntry* menu, size_t num) {
  int ind = 0;
  renderEntry(&menu[ind]);
  bool choosing = true;
  int lastState = digitalRead(CLK);
  while (choosing) {
    int currentState = digitalRead(CLK);
    if (currentState != lastState) { // Move Event
      if (digitalRead(DT) != currentState) {
          ++ind;
        } else {
          --ind;
        }
      ind = ind % num;
      renderEntry(&menu[ind]);
      lastState = currentState;
    }
    if (digitalRead(SW) == LOW) { // Read event
      choosing = false;
    }
    delay(250);
  }
  return menu[ind].target;
}
/////
MenuEntry baseMenu[] = {
  MenuEntry{"Set Colours     ",
            "                ",
            &setColour},
  MenuEntry{"Do nothing      ",
            "And Set Colours ",
            &setColour},
  MenuEntry{"Yeet All My RAM ",
            "TO Set Colours  ",
            &setColour}
};
void setup() {
  // put your setup code here, to run once:
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  lcd.begin(16, 2);
  lcd.setRGB(0, 0, 0);
  rtc.begin();  //starts rtc
  rtc.start();
  lcd.print("Take your time");
  delay(500);
  lcd.clear();
  lastState = digitalRead(CLK);  // reads the initial state of the encoder
}

void loop() {
  //
  // put your main code here, to run repeatedly:
  uint8_t sec, min, hour, day, month;
  uint16_t year;

  //get time from RTC
  rtc.get(&sec, &min, &hour, &day, &month, &year);
  char Time[16];
  sprintf(Time, "Time: %02d:%02d:%02d", hour, min, sec);  // formats time adding a 0 if <10
  char Date[16];
  sprintf(Date, "Date: %02d:%02d:%d", day, month, year);  // formats the date dd/mm/yy

  lcd.setCursor(0, 0);
  lcd.print(Time);
  lcd.setCursor(0, 1);
  lcd.print(Date);

  if (digitalRead(SW) == LOW) {
    delay(250);    
    launchTask(doMenu(baseMenu, 3));
  }
}

void setColour() {
  lcd.clear();
  uint8_t colourSet = 0;
  static char Colour[16];
  static char buff[] = "Set X: XXX";
  static const char targ[3] = {'R', 'G', 'B'};
  uint8_t cols[3] = {0};
  bool menu;
  int lastState, currentState;
  for (int ind=0; ind < 3; ++ind) {
    delay(250);
    menu = true;
    lastState = digitalRead(CLK);
    while (menu == true) {
      sprintf(Colour, "R%03d G%03d B%03d", cols[0], cols[1], cols[2]);
      sprintf(buff, "Set %c: %03d", targ[ind], colourSet);
      lcd.setCursor(0, 0);
      lcd.print(Colour);
      lcd.setCursor(0, 1);
      lcd.print(buff);
      currentState = digitalRead(CLK);                       // Current state of the encoder
      if (currentState != lastState && currentState == 1) {  // compares it once not to have ultra stroke mode
        if (digitalRead(DT) != currentState) {
          colourSet++;
        } else {
          colourSet--;
        }
      }
      lastState = currentState;
      if (digitalRead(SW) == LOW) {
        cols[ind] = colourSet;
        lcd.setRGB(cols[0], cols[1], cols[2]);
        menu = false;
      }
    }
  delay(250);
  }
}
