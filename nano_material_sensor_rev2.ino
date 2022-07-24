#include <Wire.h>
#include "Adafruit_VL53L0X.h"
#include <SPI.h>
#include <EEPROM.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#define I2C_ADDRESS 0x3C
SSD1306AsciiWire oled;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// EEPROM adresses
const byte EE_runtime = 0;
const byte EE_t_runtime = 1;
byte runtime;
int tRuntime;

const byte buttonDelay = 50;
const byte bLeft = 8;
const byte bRight = 7;
const byte led1 = 9;
const byte led2 = 10;
const byte led3 = 11;
int full = 0;
int empty = 0;
int distance = 0;
bool alert = false;
bool displayToggle = true;

// +vl53l0x
unsigned long previousMillis = 0;
const long interval = 2000;
// -vl53l0x

// +bRight
int bRightState;             // the current reading from the input pin
int lastbRightState = LOW;   // the previous reading from the input pin
unsigned long lastbRightDebounceTime = 0;  // the last time the output pin was toggled
unsigned long bRightDebounceDelay = 50;    // the debounce time; increase if the output 
// -bRight

// +bLeft
int bLeftState;             // the current reading from the input pin
int lastbLeftState = LOW;   // the previous reading from the input pin
unsigned long lastbLeftDebounceTime = 0;  // the last time the output pin was toggled
unsigned long bLeftDebounceDelay = 50;    // the debounce time; increase if the output 
// -bLeft

// +runtime
unsigned long runtimePreviousMillis = 0;
const long runtimeInterval = 3600000L;
// -runtime

// +led
unsigned long ledPreviousMillis = 0;
const long ledInterval = 150;
int ledToggle = 0;
// -led

void setup() {
  //EEPROM.update(EE_runtime, 0);
  Serial.begin(9600);
  runtime = EEPROM.read(EE_runtime);
  pinMode(bLeft, INPUT);
  pinMode(bRight, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  
  Wire.begin();
  Wire.setClock(400000L);
  oled.begin(&Adafruit128x32, I2C_ADDRESS);
  oled.setFont(Stang5x7);
  oled.setContrast(0); // 0-255
  lox.begin();
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    distance = measure.RangeMilliMeter;
  }
  full = distance;
  empty = full + 100;
}

long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both

  long result = (high<<8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}

void updateOled() {
  if (!displayToggle) {
//      oled.clear();
      oled.ssd1306WriteCmd(SSD1306_DISPLAYOFF);
    }
  else {
    oled.ssd1306WriteCmd(SSD1306_DISPLAYON);
    oled.clear();
    oled.print("Voll: ");
    oled.println(full);
    oled.print("Leer: ");
    oled.println(empty);
    oled.print("Ist:  ");
    oled.println(distance);
    oled.print("LZ: ");
    oled.print(runtime);
    oled.print(" VCC: ");
    oled.println(readVcc()/1000.0);
  }
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    VL53L0X_RangingMeasurementData_t measure;
    
    lox.rangingTest(&measure, false);
    if (measure.RangeStatus != 4) {
      distance = measure.RangeMilliMeter;
    }    
    updateOled();
  }

  // +bRight pressed
  int bRightReading = digitalRead(bRight);
  if (bRightReading != lastbRightState) {
    lastbRightDebounceTime = millis();
  }
  if ((millis() - lastbRightDebounceTime) > bRightDebounceDelay) {
    if (bRightReading != bRightState) {
      bRightState = bRightReading;
      if (bRightState == HIGH) {
        displayToggle = !displayToggle;
        updateOled();
      }
    }
  }
  lastbRightState = bRightReading;
  // -bRight pressed

  // +bLeft pressed
  int bLeftReading = digitalRead(bLeft);
  if (bLeftReading != lastbLeftState) {
    lastbLeftDebounceTime = millis();
  }
  if ((millis() - lastbLeftDebounceTime) > bLeftDebounceDelay) {
    if (bLeftReading != bLeftState) {
      bLeftState = bLeftReading;
      if (bLeftState == HIGH) {
        full = distance;
        empty = full + 100;
        updateOled();
      }
    }
  }
  lastbLeftState = bLeftReading;
  // -bLeft pressed

  if (distance >= empty) {
    unsigned long ledCurrentMillis = millis();
    if (ledCurrentMillis - ledPreviousMillis >= ledInterval) {
      ledPreviousMillis = ledCurrentMillis;
      ledToggle = !ledToggle;
      digitalWrite(led1, ledToggle);
      digitalWrite(led2, ledToggle);
      digitalWrite(led3, ledToggle);
    }
    alert = true;
  }  

  if ((alert == true) && (distance < (empty - 50))) {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    alert = false;
  }
  if (distance < full) {
    full = distance;
  }
  unsigned long runtimeCurrentMillis = millis();
  if (runtimeCurrentMillis - runtimePreviousMillis >= runtimeInterval) {
    runtimePreviousMillis = runtimeCurrentMillis;
    EEPROM.update(EE_runtime, EEPROM.read(EE_runtime) + 1);
    runtime = EEPROM.read(EE_runtime);
  }  
}
