// Somehow written by Juli/ JuliThing (on GH)
#include <Wire.h>
#include "rgb_lcd.h"
#include <DS1307.h>
#define CLK 2
#define DT 3
#define SW 4

unsigned long initialPress;         // initial start for the timer
unsigned long noPress;              // time after the button is let go
const unsigned long period = 2000;  //2 seconds in millisec form

uint8_t colourR = 0;
uint8_t colourG = 0;
uint8_t colourB = 0;
uint8_t colourSet = 0;
int currentState;
int lastState;

bool menuR = true;
bool menuG = true;
bool menuB = true;

rgb_lcd lcd;
DS1307 rtc;

void setup() {
  // put your setup code here, to run once:
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  lcd.begin(16, 2);
  lcd.setRGB(colourR, colourG, colourB);
  rtc.begin();  //starts rtc
  rtc.start();
  lcd.print("Take your time");
  delay(500);
  lcd.clear();
  lastState = digitalRead(CLK);  // reads the initial state of the encoder
}

void loop() {
  // put your main code here, to run repeatedly:
  menuR = true;
  menuG = true;
  menuB = true;  // resets backlight colour menu
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

  while (digitalRead(SW) == LOW) {
    initialPress = millis();
    if (digitalRead(SW) == HIGH && initialPress > 2000) {  // calculates the millisecs taken from press to release
      initialPress = 0;
      setColour();
    }
  }
}


void setColour() {
  lcd.clear();
  char Colour[16];
  char R[3];
  char G[3];
  char B[3];
  delay(250);
  while (menuR == true) {                                       // loop to change the value of R
    sprintf(Colour, "R%d G%d B%d", colourR, colourG, colourB);  //formats the colour values
    sprintf(R, "%03d", colourSet);
    lcd.setCursor(0, 0);
    lcd.print(Colour);
    lcd.setCursor(0, 1);
    lcd.print("Set R:");
    lcd.setCursor(7, 1);
    lcd.print(R);
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
      colourR = colourSet;
      lcd.setRGB(colourR, colourG, colourB);
      menuR = false;
    }
  }
  delay(250);
  while (menuG == true) {                                       // loop to change the value of G
    sprintf(Colour, "R%d G%d B%d", colourR, colourG, colourB);  //formats the colour values
    sprintf(G, "%03d", colourSet);
    lcd.setCursor(0, 0);
    lcd.print(Colour);
    lcd.setCursor(0, 1);
    lcd.print("Set G:");
    lcd.setCursor(7, 1);
    lcd.print(G);
    currentState = digitalRead(CLK);  // Current state of the encoder
    if (currentState != lastState && currentState == 1) {
      if (digitalRead(DT) != currentState) {
        colourSet++;
      } else {
        colourSet--;
      }
    }
    lastState = currentState;
    if (digitalRead(SW) == LOW) {
      colourG = colourSet;
      lcd.setRGB(colourR, colourG, colourB);
      menuG = false;
    }
  }
  delay(250);
  while (menuB == true) {                                       // loop to change the value of B
    sprintf(Colour, "R%d G%d B%d", colourR, colourG, colourB);  //formats the colour values
    sprintf(B, "%03d", colourSet);
    lcd.setCursor(0, 0);
    lcd.print(Colour);
    lcd.setCursor(0, 1);
    lcd.print("Set B:");
    lcd.setCursor(7, 1);
    lcd.print(B);
    currentState = digitalRead(CLK);  // Current state of the encoder
    if (currentState != lastState && currentState == 1) {
      if (digitalRead(DT) != currentState) {
        colourSet++;
      } else {
        colourSet--;
      }
    }
    lastState = currentState;
    if (digitalRead(SW) == LOW) {
      colourB = colourSet;
      lcd.setRGB(colourR, colourG, colourB);
      menuB = false;
    }
  }
}