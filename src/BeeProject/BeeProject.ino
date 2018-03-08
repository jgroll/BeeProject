#include <ADXL345.h>
#include <Wire.h>
#include <SPI.h> 
#include <SD.h>
#include "DS3231.h"

ADXL345 adxl; //variable adxl is an instance of the ADXL345 library
double lastX = 0;
double lastY = 0;
double lastZ = 0;
bool isSDPresent = true;
RTClib RTC;
double vibrationMoy = 0;
int soundMoy = 0;

// INIT
void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(12, INPUT);
  pinMode(0, INPUT);
  pinMode(2, INPUT);

  //Serial.print("Initializing SD card...");    // see if the card is present and can be initialized:   
  if (!SD.begin(4)) {
    isSDPresent = false;
    digitalWrite(5, HIGH);
  }

  adxl.powerOn();
 
  //look of activity movement on this axes - 1 == on; 0 == off 
  adxl.setActivityX(1);
  adxl.setActivityY(1);
  adxl.setActivityZ(1);

  pinMode(5, OUTPUT);
}

// Loop to send JSON to raspberry server
void loop() {
  //Serial.println("--------------------");
  String data = "";
  data += readTempeture(); 
  data += ";"; 
  data += readLid(); 
  data += ";"; 
  data += getAccelerationDelta(); 
  data += ";"; 
  data += readLoudness(); 
  data += ";"; 
  data += readDate();
  
  
  logInFile(data);
  
  samplingWait();
  
  String dataJson = "{\"temperature\":";
  dataJson += readTempeture(); 
  dataJson += ",\"hatchOpen\":"; 
  dataJson += readLid(); 
  dataJson += ",\"vibration\":"; 
  dataJson += vibrationMoy;//getAccelerationDelta(); 
  dataJson += ",\"soundActivity\":"; 
  dataJson += soundMoy;//readLoudness(); 
  dataJson += ",\"dateTime\":\""; 
  dataJson += readDate();
  dataJson += "\"}";

  Serial.println(dataJson);  
}

// Get acceleration
double getAccelerationDelta(){
  double deltaX;
  double deltaY;
  double deltaZ;
  double xyz[3];
  double ax,ay,az;
  adxl.getAcceleration(xyz);
  ax = xyz[0];
  ay = xyz[1];
  az = xyz[2];
  deltaX = abs(lastX - ax);
  deltaY = abs(lastY - ay);
  deltaZ = abs(lastZ - az);
  lastX = ax;
  lastY = ay;
  lastZ = az;
  double meanDelta = (deltaX + deltaY + deltaZ) / 3;
  return meanDelta;
}

// Average Loudness and vibration acceleration
void samplingWait()
{
  soundMoy = 0;
  vibrationMoy = 0;
  for(int i = 0 ; i < 100 ; i++)
  {
    soundMoy += readLoudness();
    vibrationMoy += getAccelerationDelta();
    delay(100);
  }
  soundMoy /= 100;
  vibrationMoy /= 100;
}

// Get Tempeture
int readTempeture(){
 int tempValue = 0;

  analogRead(2);
  tempValue = analogRead(2);
  return tempValue;
}

// Get if hatch is open
int readLid(){
  int valLid = 0;                  
  valLid = digitalRead(12);
//  if(valLid == 1){
//    Serial.println("The lid is open");
//  }else{
//    Serial.println("The lid is close");
//  }
  return valLid;
}

// Get loudness
int readLoudness(){
 int sensorValue = 0;

  analogRead(0);
  sensorValue = analogRead(0);
  return sensorValue*100/1023;
}

// Get datetime
String readDate(){
 String myString = ""; 
  DateTime now = RTC.now();
  myString = String(myString + now.day());
  
  myString = String(myString + "/" + now.month());
  myString = String(myString + "/" + now.year());
  myString = String(myString + " " + now.hour());
  myString = String(myString + ":" + now.minute());
  myString = String(myString + ":" + now.second());

  return myString;
}

// Set Data in csv file and save in SD Card
void logInFile(String data){
  digitalWrite(5, HIGH); 
  if(isSDPresent == false){
    return;     
  }
  //Serial.println("Will log : " + data);      // open the file. note that only one file can be open at a time,   
  // so you have to close this one before opening another.   
  File dataFile = SD.open("datalog.csv", FILE_WRITE);    // if the file is available, write to it:  
  if (dataFile) {     
    dataFile.println(data);     
    dataFile.close();     // print to the serial port too:     
    //Serial.println(data);
    digitalWrite(5, LOW);
 }else {    // if the file isn't open, pop up an error:   
     //Serial.println("error opening datalog.csv");
     digitalWrite(5, HIGH);   
  }
}

