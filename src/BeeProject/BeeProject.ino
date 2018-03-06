#include <Wire.h>
#include "DS3231.h"
RTClib RTC;
void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(12, INPUT);
  pinMode(0, INPUT);
  
}

void loop() {
  Serial.println("--------------------");
  printDate();
  readLid();
  readLoudness();
  delay(2000);
  
}

void readLid(){
  int valLid = 0;                  
  valLid = digitalRead(12);
  Serial.println("Ouverture :");
  if(valLid == 1){
    Serial.println("The lid is close");
  }else{
    Serial.println("The lid is open");
  }
  Serial.println(valLid);
}

void readLoudness(){
  double valLoud = 0;
  int sensorValue = 0;
  int valueFix = 0;
  float volts = 0;
  int volts_db = 0;
  int peak = 0;
  int spl_db = 0;
  analogRead(0);
  
  sensorValue = analogRead(0);

  valueFix = sensorValue ;
  volts = (5.0 / 1024.0) * valueFix;
  volts_db = 20.0 * log10(volts);
  spl_db = volts + 18;
  
  valLoud = 20 * log10(sensorValue / 5.0);
  Serial.println("Decibel :");
  Serial.println(valueFix);
}

void printDate(){
  DateTime now = RTC.now();
  Serial.print("DATE : ");
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}

