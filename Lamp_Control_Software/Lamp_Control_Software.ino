/**
  ******************************************************************************
  * Software using voice commands to control onboard directable desk lamp
  * @author  Xiang Jin
  * @brief   Software using voice commands to control onboard directable desk lamp
  ******************************************************************************
  * @note:
        Library usage code from JiapengLi 
        voice control led
  ******************************************************************************
  */
  

#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"
#include <String.h>

/**        
  Connection
  Arduino    VoiceRecognitionModule
   2   ------->     TX
   3   ------->     RX
*/
VR myVR(2,3);    // 2:RX 3:TX, you can choose your favourite pins.

uint8_t records[7]; // save record
uint8_t buf[64];


// LED array (four sections)
int led1 = 10,
    led2 = 11,
    led3 = 12,
    led4 = 13;

// Keep track of which LEDs are chosen to be controlled
int activeLED[4] = {0, 0, 0, 0};


/**
  @brief   Print signature, if the character is invisible, 
           print hexible value instead.
  @param   buf     --> command length
           len     --> number of parameters
*/
void printSignature(uint8_t *buf, int len)
{
  int i;
  for(i=0; i<len; i++){
    if(buf[i]>0x19 && buf[i]<0x7F){
      Serial.write(buf[i]);
    }
    else{
      Serial.print("[");
      Serial.print(buf[i], HEX);
      Serial.print("]");
    }
  }
}

/**
  @brief   Print signature, if the character is invisible, 
           print hexible value instead.
  @param   buf  -->  VR module return value when voice is recognized.
             buf[0]  -->  Group mode(FF: None Group, 0x8n: User, 0x0n:System
             buf[1]  -->  number of record which is recognized. 
             buf[2]  -->  Recognizer index(position) value of the recognized record.
             buf[3]  -->  Signature length
             buf[4]~buf[n] --> Signature
*/
void printVR(uint8_t *buf)
{
  Serial.println("VR Index\tGroup\tRecordNum\tSignature");

  Serial.print(buf[2], DEC);
  Serial.print("\t\t");

  if(buf[0] == 0xFF){
    Serial.print("NONE");
  }
  else if(buf[0]&0x80){
    Serial.print("UG ");
    Serial.print(buf[0]&(~0x80), DEC);
  }
  else{
    Serial.print("SG ");
    Serial.print(buf[0], DEC);
  }
  Serial.print("\t");

  Serial.print(buf[1], DEC);
  Serial.print("\t\t");
  if(buf[3]>0){
    printSignature(buf+4, buf[3]);
  }
  else{
    Serial.print("NONE");
  }
  Serial.println("\r\n");
}


void setup()
{

  // Start serial connection to voice recognition module
  myVR.begin(9600);
  
  Serial.begin(115200);
  Serial.println("Elechouse Voice Recognition V3 Module\r\nControl LED sample");
  
  pinMode(led, OUTPUT);
    
  if(myVR.clear() == 0){
    Serial.println("Recognizer cleared.");
  }else{
    Serial.println("Not find VoiceRecognitionModule.");
    Serial.println("Please check connection and restart Arduino.");
    while(1);
  }
  
  // Load recordings for on (records 0 to 3)
  for (int i = 0; i <= 3; i++) {

    if (myVR.load((uint8_t)i) >= 0)
      Serial.println("On function: Record " + String(i) + " recorded.");
  }

  // Load recordings for off (records 4 to 7)
  for (int i = 4; i <= 7; i++) {

    if (myVR.load((uint8_t)i) >= 0)
      Serial.println("Off function: Record " + String(i) + " recorded.");
  }

  // Add more commands here as we go...
}

void loop()
{
  int ret;
  ret = myVR.recognize(buf, 50);

  // Check if voice command received
  if(ret>0){

    if (buf[1] >= 0 && buf[1] <= 3) {   // Check for on command

      digitalWrite(led, HIGH);
    } else if (buf[1] >= 4 && buf[1] <= 7) {  // Check for off command

      digitalWrite(led, LOW);
    } else {

      Serial.println("Record function undefined");
    }
    /** voice recognized */
    printVR(buf);
  }
}



