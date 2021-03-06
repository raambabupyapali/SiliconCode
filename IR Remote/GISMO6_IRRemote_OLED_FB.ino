/*
 * IRremoteESP8266: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * This is very simple teaching code to show you how to use the library.
 * If you are trying to decode your Infra-Red remote(s) for later replay,
 * use the IRrecvDumpV2.ino (or later) example code instead of this.
 * An IR detector/demodulator must be connected to the input kRecvPin.
 * Copyright 2009 Ken Shirriff, http://arcfn.com
 * Example circuit diagram:
 *  https://github.com/crankyoldgit/IRremoteESP8266/wiki#ir-receiving
 * Changes:
 *   Version 0.2 June, 2017
 *     Changed GPIO pin to the same as other examples.
 *     Used our own method for printing a uint64_t.
 *     Changed the baud rate to 115200.
 *   Version 0.1 Sept, 2015
 *     Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009
 */

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include "Credentials.h"

// An IR detector/demodulator is connected to GPIO pin 16(D5 on a NodeMCU
// board).
// Note: GPIO 16 won't work on the ESP8266 as it does not have interrupts.
const uint16_t kRecvPin = 4;

IRrecv irrecv(kRecvPin);

decode_results results;

void OLEDInit();
void OLEDUpdate();
void WiFiInit();
void FirebaseInit();
void FirebaseWrite();
void FirebaseRead();

bool controlMode=0;
bool controlStatus=0;
String modeDisplay="LOCAL";
String statusDisplay="OFF";

String tag1 = "IOTLAB/IR/Mode";
String tag2 = "IOTLAB/IR/Status";

String motorCmdFull;
String motorCmd;
int finalCmd;


void setup() {
  pinMode(2,OUTPUT);
  pinMode(13,OUTPUT);
  Serial.begin(115200);
  irrecv.enableIRIn();  // Start the receiver
  while (!Serial)  // Wait for the serial connection to be establised.
    delay(50);
  Serial.println();
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(kRecvPin);

  OLEDInit();
  WiFiInit();
  FirebaseInit();
}

void loop() {
  if (irrecv.decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    long btnValue = results.value;
    serialPrintUint64(btnValue);
    Serial.println("");
    switch(btnValue){
      case 16718055:
      case 16736925:
      case 16712445:  
      Serial.println("Up arrow");
      if(controlMode == 0){
      digitalWrite(2,HIGH);
      digitalWrite(13,HIGH);
      controlStatus = 1;
      statusDisplay = "ON";}
      break;
      case 16730805:
      case 16754775:
      case 16720605:  
      Serial.println("Down arrow");
      if(controlMode == 0){
      digitalWrite(2,LOW);
      digitalWrite(13,LOW);
      controlStatus = 0;
      statusDisplay = "OFF";}
      break;
      case 16738455:
      case 16728765:
      case 16753245:  
      Serial.println("Local Control");
      controlMode = 0;
      modeDisplay = "LOCAL";
      break;
      case 16756815:
      case 16732845:
      case 16769565:  
      Serial.println("Cloud Control");
      controlMode = 1;
      modeDisplay = "CLOUD";
      controlStatus =0;
      statusDisplay = "OFF";
      digitalWrite(2,LOW);
      digitalWrite(13,LOW);
      Firebase.setString(firebaseData,"IOTLAB/IR/Cloud_Cmd","000");
      break;
      default:
      Serial.println("Not a valid key");
      break;
    }
    irrecv.resume();  // Receive the next value
    
  }
if(controlMode){
Firebase.getString(firebaseData,"IOTLAB/IR/Cloud_Cmd",motorCmdFull);
motorCmd = motorCmdFull.substring(2,motorCmdFull.length()-2);
finalCmd = motorCmd.toInt();

if(finalCmd == 1)
{
  digitalWrite(2,HIGH);
  digitalWrite(13,HIGH);
  Serial.println("Appliance ON");
  statusDisplay = "ON";
}
else
{
  digitalWrite(2,LOW);
  digitalWrite(13,LOW);
  Serial.println("Applinace OFF");
  statusDisplay = "OFF";
  
}
}
  OLEDUpdate();
  FirebaseWrite();
  delay(1000);
}
void OLEDInit(){
  display.init();
  display.setFont(ArialMT_Plain_24);
}

void OLEDUpdate(){
  display.clear();
  display.drawString(30,0,modeDisplay);
  display.drawString(40,30,statusDisplay);
  display.display();
}
void WiFiInit(){
  pinMode(2,OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    digitalWrite(2,!digitalRead(2));
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
   
}
void FirebaseInit(){
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

}
void FirebaseWrite(){
  Firebase.setString(firebaseData,tag1,modeDisplay);
  Firebase.setString(firebaseData,tag2,statusDisplay); 
}

  
