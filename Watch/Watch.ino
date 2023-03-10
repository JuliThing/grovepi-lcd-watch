// somehow written by Juli / JuliThing (GH)
//#include <CapacitiveSensor.h> //paulstoffregen //Experimental
#include <ArduinoLowPower.h>
#include <StopWatch.h> // StopWatch_RT
#include <Wire.h>
#include <rgb_lcd.h>
#include <DS1307.h>
#define CLK 3 // wiring changed
#define DT 4
#define SW 2
#define Buzzer 5

unsigned long initial;         // initial start for the timer
unsigned long lastPress;
unsigned long lastCapPress;
long capButton; //assigns the capacitive button var
const unsigned long period = 2000;  //2 seconds in millisec form
volatile int sleepCount = 1;

uint8_t colourR = 0;
uint8_t colourG = 0;
uint8_t colourB = 0;
uint8_t colourSet = 0;
uint8_t menuSelect = 1;
int currentState;
int lastState;

bool timerStart = false;
bool menuOpen = true;
bool watchRunning = false;
bool watchMenu = false;
bool menuR = true;
bool menuG = true;
bool menuB = true;

rgb_lcd lcd;
DS1307 rtc;
StopWatch watch;
//CapacitiveSensor cs_11_9 = CapacitiveSensor(11,9); //sets pin 9 as the capacitive sensor pin //Experimental

byte blank[8] = { //creates an arrow icon 
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

byte arrow[8] = { //creates an arrow icon 
  0b00000,
  0b00100,
  0b00010,
  0b11111,
  0b00010,
  0b00100,
  0b00000,
  0b00000
};

void setup() {
  // put your setup code here, to run once:
  //cs_11_9.set_CS_AutocaL_Millis(0xFFFFFFFF);  //Calibrate the sensor // Experimental capacitive sensor
  pinMode(Buzzer, OUTPUT);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  lcd.begin(16, 2);
  lcd.setRGB(colourR, colourG, colourB);
  lcd.createChar(1, blank); // creates an empty icon
  lcd.createChar(2, arrow); // turns the custom arrow icon into a character
  rtc.begin();  //starts rtc
  rtc.start();
  lcd.setCursor(0, 0);
  lcd.print("Take your time");
  delay(1000);
  lcd.clear();
  lastState = digitalRead(CLK);  // reads the initial state of the encoder
  initial = millis();
  LowPower.attachInterruptWakeup(SW, noslep, CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:
  menuOpen = true;
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
    lastPress = millis();
    if (digitalRead(SW) == HIGH && initial - lastPress > 2000) {  // calculates the millisecs taken from press to release
      lastPress = 0;
      selectMenu();
     }
    }
  }


//void touchButton(){
//  cs_11_9.set_CS_AutocaL_Millis(0xFFFFFFFF);  //Calibrate the sensor...
//  capButton = cs_11_9.capacitiveSensor(30); // capacitance of the button
//} //Experimental 

void counterMenu(){
  currentState = digitalRead(CLK);                       // Current state of the encoder
    if (currentState != lastState && currentState == 1) {  // compares it once not to have ultra stroke mode
      if (digitalRead(DT) != currentState) {
        menuSelect++;
        tone(Buzzer, 400);
        delay(50);
        noTone(Buzzer);
      } else {
        menuSelect--;
        tone(Buzzer, 400);
        delay(50);
        noTone(Buzzer);
      }
    }
    lastState = currentState;
    menuSelect = constrain(menuSelect, 0, 5);
    if (menuSelect == 5){
      menuSelect = 1;
    }
    if (menuSelect == 0){
        menuSelect = 4;
    }
}

void colourVal(){
  currentState = digitalRead(CLK);                       // Current state of the encoder
  if (currentState != lastState && currentState == 1) {  // compares it once not to have ultra stroke mode
    if (digitalRead(DT) != currentState) {
      colourSet+=5;
    } else {
      colourSet-=5;
    }
  }
    lastState = currentState;
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
    colourVal();
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
    colourVal();
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
    colourVal();
    if (digitalRead(SW) == LOW) {
      colourB = colourSet;
      lcd.setRGB(colourR, colourG, colourB);
      menuB = false;
    }
  }
}

void stopwatch(){
  delay(200);
  lcd.clear();
  while (watchMenu == true){
    //touchButton();
    uint32_t timeElapsed = watch.elapsed();
    uint16_t millisecs = watch.elapsed();
    millisecs = constrain(millisecs, 0, 1000);
    lcd.setCursor(0,0);
    lcd.print(timeElapsed);
    lcd.setCursor(0,1);
    lcd.print(capButton);
    /*while (capButton > 180){ // reads the switch and starts the watch
    touchButton();
    if (watch.isRunning() == true){
        watch.start();
        delay(20);
    } else{
      watch.stop();
      delay(20);
    }*///Experimental/ Don't use Stopwatch as it is broken 
  }
  while (digitalRead(SW) == LOW && watch.isRunning() == false){
    lastCapPress = millis();
    if (digitalRead(SW) == HIGH && initial - lastCapPress > 2000){
      watch.reset();
      watchMenu = false;
    } else{
      watch.reset();
      lastCapPress = 0;
    }
  }
}

void slep(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Zzzz...");
  delay(300);
  LowPower.deepSleep(); // puts the arduino to sleep^2 works as expected
}

void noslep(){
  sleepCount ++; //idk why
}

void menu1(){
  lcd.setCursor(1,0);
  lcd.print("Colour Set");
  lcd.setCursor(1,1);
  lcd.print("Stop Watch");
}

void menu2(){
  lcd.setCursor(1,0);
  lcd.print("Settings  ");
  lcd.setCursor(1,1);
  lcd.print("Sleep     ");
}


void selectMenu(){
  lcd.clear();
  while (menuOpen == true){
    counterMenu();
    switch(menuSelect){
      case 1:
      menu1();
      lcd.setCursor(0, 1);
      lcd.write(1); // writes an empty character to clear 0,1
      lcd.setCursor(0, 0);
      lcd.write(2);
      break;
      case 2:
      menu1();
      lcd.setCursor(0, 0);
      lcd.write(1); // writes an empty character to clear 0,0 
      lcd.setCursor(0,1); 
      lcd.write(2);
      break;
      case 3:
      menu2();
      lcd.setCursor(0, 1);
      lcd.write(1); // writes an empty character to clear 0,1
      lcd.setCursor(0, 0);
      lcd.write(2);
      break;
      case 4:
      menu2();
      lcd.setCursor(0, 0);
      lcd.write(1); // writes an empty character to clear 0,0 
      lcd.setCursor(0,1); 
      lcd.write(2);  
      break;    
    }
    if (digitalRead(SW) == LOW){
      menuOpen = false;
      switch(menuSelect){
        case 1:
        setColour();
        break;
        case 2:
        watchMenu = true;
        stopwatch();
        break;
        case 3:
        break;
        case 4:
        slep();
        break;
      }
    }
  }
}
