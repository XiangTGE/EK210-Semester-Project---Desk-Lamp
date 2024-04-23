/**
  ******************************************************************************
  * Software using voice commands to control onboard directable desk lamp
  * @author  Xiang Jin, Alex Weintraub
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
 *  - "Lamp" (records 0 to 2)
 * 
 * Brightness commands:
 *  - "Low" - 33% brightness (records 3 to 5)
 *  - "Medium" - 66% brightness (records 6 to 8)
 *  - "High" - 100% brightness (records 9 to 11)
 *
 * Timer Aspect:
 *  - User will have option to turn on/off the timer, which sets lamp to turn off by itself after a set period of time
 *  - Timer will reset whenever the motion sensor is triggered
 *  - If lamp is off, it will turn on again after motion sensor is triggered
 *
 * Orientation Commands:
 *  - Quadrant I ("one") - records 12 to 14
 *  - Quadrant II ("two") - records 15 to 17
 *  - Quadrant III ("three") - records 18 to 20
 *  - Quadrant IV ("four") - records 21 to 23
*/

#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"


/**        
  Connection
  Arduino    VoiceRecognitionModule
   3   ------->     TX
   2   ------->     RX
*/

// Keep track of whether lamp is turned on or off
int lamp_on = 0;

// LED array (four sections) - Each entry is PWM pin
int led[4] = {6, 9, 10, 11};

// Keep track of which LEDs are chosen to be armed/unarmed
// 1 means armed, 0 means not armed
int armedLED[4] = {0, 0, 0, 0};


// Motion sensor pins
int motion_sensor = 4;
int motion_detected = 0;


void setup()
{
  
  Serial.begin(115200);
  Serial.println("EK210 Directable Desk Lamp 1.1");
  

  // Initialize LEDs
  for (int i = 0; i < sizeof(led)/sizeof(int); i++) {

    pinMode(led[i], OUTPUT);
  }


  // Declare motion sensor as input
  pinMode(motion_sensor, INPUT);
}

void loop()
{
  // Determine if motion sensor is activated; if it is, then turn reset timer
  motion_detected = digitalRead(motion_sensor);
  if (motion_detected) {
    // Reset timer when motion is detected
    led_on();
  } else {

    led_off();
  }
}



// Turn all LEDs on
void led_on () {

  for (int i = 0; i < 4; i++) {

    analogWrite(led[i], 100);
  }
}


// Turn all LEDs off
void led_off () {

  for (int i = 0; i < 4; i++) {

    analogWrite(led[i], 0);
  }
}
