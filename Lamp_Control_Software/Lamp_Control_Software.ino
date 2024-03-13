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
  

/*
 * Directable desk lamp features:
 *  - On/off: 
 *  - Brightness levels: 1-4 from 25% to 100% in increments of 25%
 *  - Timer: Dictate when to turn lamp off if this mode is turned on
 *  - Orientation: Has four section in LED array that can be turned on independently of each other
 *
 * On/off commands:
 *  - "Lights on" (records 0 to 3)
 *  - "Lights off" (records 4 to 7)
 * 
 * Brightness commands:
 *  - "Brightness 1" - 25% brightness (records 8 to 11)
 *  - "Brightness 2" - 50% brightness (records 12 to 15)
 *  - "Brightness 3" - 75% brightness (records 16 to 19)
 *  - "Brightness 4" - 100% brightness (records 20 to 23)
 *
 * Timer Commands:
 *  - 
 *
 * Orientation Commands:
 *  - 
*/

#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"


/**        
  Connection
  Arduino    VoiceRecognitionModule
   2   ------->     TX
   3   ------->     RX
*/
VR myVR(2,3);    // 2:RX 3:TX, you can choose your favourite pins.

uint8_t records[7]; // save record
uint8_t buf[64];


// LED array (four sections) - Each entry is PWM pin
int led[4] = {3, 5, 7, 9};

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
  Serial.println("EK210 Directable Desk Lamp 1.0");
  

  // Initialize LEDs
  for (int i = 0; i < sizeof(led)/sizeof(int); i++) {

    pinMode(led[i], OUTPUT);
  }

  // Diag mssg for LEDs
  for (int i = 0; i <= 3; i++) {

    Serial.println("LED " + led[i] + String(" loaded."));
  }
    

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
  int ret = myVR.recognize(buf, 50);

  // Check if voice command received
  if(ret>0){

    if (buf[1] >= 0 && buf[1] <= 3) {           // Check for on command

      // Turn on all controllable segments
      led_on_off("on");
    } else if (buf[1] >= 4 && buf[1] <= 7) {    // Check for off command

      // Turn off all controllable segments
      led_on_off("off");
    } else if (buf[1] >= 8 && buf[1] <= 23) {   // Check for brightness command

      // Swtiches brightness based on input
      if (buf[1] >= 8 && buf[1] <= 11)
        brightness_control(1);

      else if (buf[1] >= 12 && buf[1] <= 15)
        brightness_control(2);

      else if (buf[1] >= 16 && buf[1] <= 19)
        brightness_control(3);

      else if (buf[1] >= 20 && buf[1] <= 23)
        brightness_control(4);

    } else if (buf[1] >= 24 && buf[1] <= 39) {  // Check for orientation command


    } else {

      Serial.println("Unexpected error: no command found");
    }

    /** voice recognized */
    printVR(buf);
  }
}


// Toggle controllable LED segments on/off
// "toggle" can be "on" or "off"
void led_on_off (String toggle) {

  int numLEDs = sizeof(activeLED) / sizeof(int);

  for (int i = 0; i < numLEDs; i++) {

    if (activeLED[i] == 1 && toggle == "on")
      digitalWrite(led[i], HIGH);
    
    else if (activeLED[i] == 1 && toggle == "off")
      digitalWrite(led[i], LOW);
  }
}


// Controls brightness of LEDs
// Four levels, 1-4
// Each segment controlled by pins 3, 5, 6, 9 (these are PWM pins)
void brightness_control (int level) {

  int duty_cycle = 0; // Duty cycle out of 255

  // Determine duty cycle
  switch (level) {

    case 1: // 25%
      duty_cycle = 64;
      break;

    case 2: // 50%
      duty_cycle = 127;
      break;

    case 3:
      duty_cycle = 191;
      break;

    case 4: // 100%
      duty_cycle = 255;
      break;
  }

  // Switch LEDs to this brightness
  for (int i = 0; i < sizeof(led)/sizeof(int); i++) {

    if (activeLED[i] == 1)
      analogWrite(led[i], duty_cycle);
  }
}


// Control which LED segments are active
void orientation_control (int led_segment, ) {
  
  
}
