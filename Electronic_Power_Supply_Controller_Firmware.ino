#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_ADS1015.h>
Adafruit_MCP4725 voltageDAC;
Adafruit_MCP4725 currentDAC;
Adafruit_ADS1115 outputADC(0x48);
LiquidCrystal_I2C lcd(0x27, 20, 4);
#define clk 2 
#define data 4 
#define sw 3
const float voltageLimit = 30.0;
const float currentLimit = 3.5;
int16_t voltagesense, currentsense;
int screen = 0;
int arrowpos = 0;
int cursorpos = 0;
int cursoroffset = 0;
int setOutputVoltage = 0;
int setOutputCurrent = 0;
float outputVoltage = 0.0;
float setVoltage = 0.0;
float outputCurrent = 0.0;
float setCurrentLimit = 0.0;
float outputPower = 0.0;
float counter = 0.0;
int State = 0;
int LastState;
volatile boolean TurnDetected = false;
volatile boolean up = false;
volatile boolean button = false;
volatile boolean powermode = false;
byte customChar[8] = {
  0b00000,
  0b00100,
  0b00110,
  0b11111,
  0b00110,
  0b00100,
  0b00000,
  0b00000
};
byte customChar1[8] = {
  0b00100,
  0b01110,
  0b11111,
  0b00000,
  0b00000,
  0b11111,
  0b01110,
  0b00100,
};
byte customChar2[8] = {
  0b11111,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
};
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, customChar);
  lcd.createChar(1, customChar1);
  lcd.createChar(2, customChar2);
  pinMode(clk, INPUT);
  pinMode(data, INPUT);  
  pinMode(sw, INPUT_PULLUP);
  voltageDAC.begin(0x60);
  currentDAC.begin(0x61);
  outputADC.begin();
  attachInterrupt(digitalPinToInterrupt(clk), isr0_encoder, LOW);
  attachInterrupt(digitalPinToInterrupt(sw), isr1_button, FALLING);
  lcd.clear();
  screen0();
  lcd.setCursor(0,0);
  lcd.write(0);
  voltageDAC.setVoltage(0, false);
  currentDAC.setVoltage(0, false);
}

void loop() {
  if (powermode){
    voltagesense = outputADC.readADC_SingleEnded(0);
    currentsense = outputADC.readADC_SingleEnded(1);
    outputVoltage = (((voltagesense*0.1875)/1000)*(5.0/5000.0)*(30.0/5.0));
    outputCurrent = (((currentsense*0.1875)/1000)*(5.0/5000.0)/(4*0.33));
    outputPower = outputVoltage*outputCurrent;
    lcd.setCursor(11,0);
    lcd.print(outputVoltage, 2);
    lcd.print("V ");
    lcd.setCursor(11,1);
    lcd.print(outputCurrent, 2);
    lcd.print("A ");
    lcd.setCursor(9,2);
    lcd.print(outputPower, 2);
    lcd.print("W ");
    delay(400);
  }
  if (TurnDetected){
    delay(300);
    switch(screen){
      case 0:
      {
        screen = 0;
        switch(arrowpos){
          case 0:
          {
            screen0();
            if (up){
              lcd.setCursor(0,1);
              lcd.write(0);
              arrowpos = 1;
            }else{
              lcd.setCursor(2,2);
              lcd.write(0);
              arrowpos = 2;
            }
            break;
          }
          case 1:
          {
            screen0();
            if (up){
              lcd.setCursor(2,2);
              lcd.write(0);
              arrowpos = 2;
            }else{
              lcd.setCursor(0,0);
              lcd.write(0);
              arrowpos = 0;
            }
            break;
          }
          case 2:
          {
            screen0();
            if (up){
              lcd.setCursor(0,0);
              lcd.write(0);
              arrowpos = 0;
            }else{
              lcd.setCursor(0,1);
              lcd.write(0);
              arrowpos = 1;
            }
            break;
          }
        }
        break;
      }
      case 1:
      {
        screen = 1;
        switch(arrowpos){
          case 0:
          {
            screen1();
            lcd.setCursor(2,2);
            lcd.write(0);
            arrowpos = 1;
            break;
          }
          case 1:
          {
            screen1();
            lcd.setCursor(2,1);
            lcd.write(0);
            arrowpos = 0;
            break;
          }
        }
        break;
      }
      case 2:
      {
        if (setVoltage < 10.0){
          cursoroffset = 0;
        }else{
          cursoroffset = 1;
        }
        switch(arrowpos){
          screen = 2;
          case 0:
          {
            screen2();
            if (up){
              lcd.setCursor(13+cursoroffset,2);
              lcd.write(2);
              arrowpos = 1;
            }else{
              lcd.setCursor(1,2);
              lcd.write(0);
              arrowpos = 3;
            }
            break;
          }
          case 1:
          {
            screen2();
            if (up){
              lcd.setCursor(14+cursoroffset,2);
              lcd.write(2);
              arrowpos = 2;
            }else{
              lcd.setCursor(11+cursoroffset,2);
              lcd.write(2);
              arrowpos = 0;
            }
            break;
          }
          case 2:
          {
            screen2();
            if (up){
              lcd.setCursor(1,2);
              lcd.write(0);
              arrowpos = 3;
            }else{
              lcd.setCursor(13+cursoroffset,2);
              lcd.write(2);
              arrowpos = 1;
            }
            break;
          }
          case 3:
          {
            screen2();
            if (up){
              lcd.setCursor(11+cursoroffset,2);
              lcd.write(2);
              arrowpos = 0;
            }else{
              lcd.setCursor(14+cursoroffset,2);
              lcd.write(2);
              arrowpos = 2;
            }
            break;
          }
        }
        break;
      }
      case 3:
      {
        screen = 3;
        switch(arrowpos){
          case 0:
          {
            screen3();
            lcd.setCursor(2,2);
            lcd.write(0);
            arrowpos = 1;
            break;
          }
          case 1:
          {
            screen3();
            lcd.setCursor(2,1);
            lcd.write(0);
            arrowpos = 0;
            break;
          }
        }
        break;
      }
      case 4:
      {
        switch(arrowpos){
          screen = 4;
          case 0:
          {
            screen4();
            if (up){
              lcd.setCursor(13,2);
              lcd.write(2);
              arrowpos = 1;
            }else{
              lcd.setCursor(1,2);
              lcd.write(0);
              arrowpos = 3;
            }
            break;
          }
          case 1:
          {
            screen4();
            if (up){
              lcd.setCursor(14,2);
              lcd.write(2);
              arrowpos = 2;
            }else{
              lcd.setCursor(11,2);
              lcd.write(2);
              arrowpos = 0;
            }
            break;
          }
          case 2:
          {
            screen4();
            if (up){
              lcd.setCursor(1,2);
              lcd.write(0);
              arrowpos = 3;
            }else{
              lcd.setCursor(13,2);
              lcd.write(2);
              arrowpos = 1;
            }
            break;
          }
          case 3:
          {
            screen4();
            if (up){
              lcd.setCursor(11,2);
              lcd.write(2);
              arrowpos = 0;
            }else{
              lcd.setCursor(14,2);
              lcd.write(2);
              arrowpos = 2;
            }
            break;
          }
        }
        break;
      }
      case 6:
      {
        if (up){
          setVoltage = setVoltage + counter;
          if (setVoltage > voltageLimit){
            setVoltage = 0;
            screen2();
          }
        }else{
          setVoltage = setVoltage - counter;
          if (setVoltage < 0){
            setVoltage = voltageLimit;
          }
        }
        setOutputVoltage = setVoltage*(4095/voltageLimit);
        lcd.setCursor(11,1);
        lcd.print(setVoltage, 2);
        lcd.print("V");
        lcd.write(1);
        break;
      }
      case 7:
      {
        if (up){
          setCurrentLimit = setCurrentLimit + counter;
          if (setCurrentLimit > currentLimit){
            setCurrentLimit = 0.0;
          }
        }else{
          setCurrentLimit = setCurrentLimit - counter;
          if (setCurrentLimit < 0){
            setCurrentLimit = currentLimit;
          }
        }
        setOutputCurrent = setCurrentLimit*(1000/currentLimit);
        lcd.setCursor(11,1);
        lcd.print(setCurrentLimit, 2);
        lcd.print("A");
        lcd.write(1);
        break;
      }
    }
    TurnDetected = false;
  }
  if (button){
    Serial.println(screen);
    delay(300);
    switch(screen){
      case 0:
      {
        switch(arrowpos){
          case 0:
          {
            screen1();
            lcd.setCursor(2,1);
            lcd.write(0);
            screen = 1;
            arrowpos = 0;
            break;
          }
          case 1:
          {
            screen3();
            lcd.setCursor(2,1);
            lcd.write(0);
            screen = 3;
            arrowpos = 0;
            break;
          }
          case 2:
          {
            screen5();
            powermode = true;
            screen = 5;
            arrowpos = 0;
            break;
          }
        }
        break;
      }
      case 1:
      {
        switch(arrowpos){
          case 0:
          {
            screen2();
            lcd.setCursor(11,2);
            lcd.write(2);
            screen = 2;
            arrowpos = 0;
            cursorpos = 0;
            break;
          }
          case 1:
          {
            screen0();
            lcd.setCursor(0,0);
            lcd.write(0);
            screen = 0;
            arrowpos = 0;
            break;
          }
        }
        break;
      }
      case 2:
      {
        screen = 6;
        if (setVoltage < 10.0){
          lcd.setCursor(16,1);
          lcd.write(1);
        }else{
          lcd.setCursor(17,1);
          lcd.write(1);
        }
        switch(arrowpos){
          case 0:
          {
            counter = 1;
            break;
          }
          case 1:
          {
            counter = 0.1;
            break;
          }
          case 2:
          {
            counter = 0.01;
            break;
          }
          case 3:
          {
            screen1();
            lcd.setCursor(2,1);
            lcd.write(0);
            screen = 1;
            arrowpos = 0;
            break;
          }
        }
        break;
      }
      case 3:
      {
        switch(arrowpos){
          case 0:
          {
            screen4();
            lcd.setCursor(11,2);
            lcd.write(2);
            screen = 4;
            arrowpos = 0;
            break;
          }
          case 1:
          {
            screen0();
            lcd.setCursor(0,1);
            lcd.write(0);
            screen = 0;
            arrowpos = 1;
            break;
          }
        }
        break;
      }
      case 4:
      {
        screen = 7;
        lcd.setCursor(16,1);
        lcd.write(1);
        switch(arrowpos){
          case 0:
          {
            counter = 1;
            break;
          }
          case 1:
          {
            counter = 0.1;
            break;
          }
          case 2:
          {
            counter = 0.01;
            break;
          }
          case 3:
          {
            screen3();
            lcd.setCursor(2,1);
            lcd.write(0);
            screen = 3;
            arrowpos = 0;
            break;
          }
        }
        break;
      }
      case 5:
      {
        screen0();
        lcd.setCursor(2,2);
        lcd.write(0);
        powermode = false;
        screen = 0;
        arrowpos = 2;
        break;
      }
      case 6:
      {
        screen2();
        if (setVoltage < 10.0){
          cursoroffset = 0;
        }else{
          cursoroffset = 1;
        }
        switch(arrowpos){
          case 0:
          {
            lcd.setCursor(11+cursoroffset,2);
            lcd.write(2);
            arrowpos = 0;
            screen = 2;
            break;
          }
          case 1:
          {
            lcd.setCursor(13+cursoroffset,2);
            lcd.write(2);
            arrowpos = 1;
            screen = 2;
            break;
          }
          case 2:
          {
            lcd.setCursor(14+cursoroffset,2);
            lcd.write(2);
            arrowpos = 2;
            screen = 2;
            break;
          }
        }
        break;
      }
      case 7:
      {
        screen4();
        switch(arrowpos){
          case 0:
          {
            lcd.setCursor(11,2);
            lcd.write(2);
            arrowpos = 0;
            screen = 4;
            break;
          }
          case 1:
          {
            lcd.setCursor(13,2);
            lcd.write(2);
            arrowpos = 1;
            screen = 4;
            break;
          }
          case 2:
          {
            lcd.setCursor(14,2);
            lcd.write(2);
            arrowpos = 2;
            screen = 4;
            break;
          }
        }
        break;
      }
    }
    button = false;
    voltageDAC.setVoltage(setOutputVoltage, false);
    currentDAC.setVoltage(setOutputCurrent, false);
  }
}
void isr0_encoder(){
  TurnDetected = true;
  static unsigned long LastInterruption = 0;
  unsigned long TimeInterruption = millis();
  if (TimeInterruption - LastInterruption > 10){
    if (digitalRead(data) == HIGH){
      up = true;
    }else{
      up = false;
    }
    LastInterruption = TimeInterruption;
  }
}
void isr1_button(){
  if (!digitalRead(sw)){
    button = true;
  }else{
    button = false;
  }
}
void screen0(){
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Set Output Voltage");
  lcd.setCursor(1,1);
  lcd.print("Set Current Limit");
  lcd.setCursor(3,2);
  lcd.print("Measure Output");
}
void screen1(){
  lcd.clear();
  lcd.setCursor(3,1);
  lcd.print("Voltage: ");
  lcd.print(setVoltage, 2);
  lcd.print("V");
  lcd.setCursor(3,2);
  lcd.print("Back");
}
void screen2(){
  lcd.clear();
  lcd.setCursor(2,1);
  lcd.print("Voltage: ");
  lcd.print(setVoltage, 2);
  lcd.print("V");
  lcd.setCursor(2,2);
  lcd.print("Back");
}
void screen3(){
  lcd.clear();
  lcd.setCursor(3,1);
  lcd.print("Current: ");
  lcd.print(setCurrentLimit, 2);
  lcd.print("A");
  lcd.setCursor(3,2);
  lcd.print("Back");
}
void screen4(){
  lcd.clear();
  lcd.setCursor(2,1);
  lcd.print("Current: ");
  lcd.print(setCurrentLimit, 2);
  lcd.print("A");
  lcd.setCursor(2,2);
  lcd.print("Back");
}
void screen5(){
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Voltage: ");
  lcd.setCursor(2,1);
  lcd.print("Current: ");
  lcd.setCursor(2,2);
  lcd.print("Power: ");
}
