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
VR myVR(2,3);    // 2:RX 3:TX, you can choose your favourite pins.

uint8_t records[7]; // save record
uint8_t buf[64];

// Keep track of whether lamp is turned on or off
int lamp_on = 0;

// LED array (four sections) - Each entry is PWM pin
int led[4] = {6, 9, 10, 11};

// Keep track of which LEDs are chosen to be armed/unarmed
// 1 means armed, 0 means not armed
int armedLED[4] = {0, 0, 0, 0};

// Duty cycle of LEDs when turned on
int maxBrightness = 100;
int ledDutyCycle = maxBrightness;


// Motion sensor pins
int motion_sensor = 4;
int motion_detected = 0;

// timer ints
int startTime = 0;
int timerDuration = 30 * 60 * 1000; // 30 minutes in milliseconds

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
  Serial.println("EK210 Directable Desk Lamp 1.1");
  

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
  

  // Load recordings for on (records 0 to 2)
  for (int i = 0; i <= 2; i++) {

    if (myVR.load((uint8_t)i) >= 0)
      Serial.println("Lamp toggle function: Record " + String(i) + " recorded.");
  }


  // Load brightness commands ()
  for (int i = 6; i <= 14; i++) {

    if (myVR.load((uint8_t)i) >= 0)
      Serial.println("Brightness Command: Record " + String(i) + " recorded.");
  }


  // Declare motion sensor as input
  pinMode(motion_sensor, INPUT);
}

void loop()
{
  int ret = myVR.recognize(buf, 50);

  // Check if voice command received
  if(ret>0) {

    if (buf[1] >= 0 && buf[1] <= 3) {           // Check for on/off command

      // Turn off all controllable segments
      led_on_off();
    } else if (buf[1] >= 3 && buf[1] <= 11) {   // Check for brightness command

      // Swtiches brightness based on input
      if (buf[1] >= 3 && buf[1] <= 5)
        brightness_control(1);

      else if (buf[1] >= 6 && buf[1] <= 8)
        brightness_control(2);

      else if (buf[1] >= 9 && buf[1] <= 11)
        brightness_control(3);
        
    } else if (buf[1] >= 12 && buf[1] <= 23) {  // Check for orientation command
    
      if (buf[1] >= 12 && buf[1] <= 14)
        armedLED[0] = !armedLED[0];

      else if (buf[1] >= 15 && buf[1] <= 17)
        armedLED[1] = !armedLED[1];

      else if (buf[1] >= 18 && buf[1] <= 20)
        armedLED[2] = !armedLED[2];

      else if (buf[1] >= 21 && buf[1] <= 23)
        armedLED[3] = !armedLED[3];
    } else {

      Serial.println("Unexpected error: no command found");
    }

    /** voice recognized */
    printVR(buf);

    // reset timer whenever voive command is called
    startTime = millis(); 
  }


  // Determine if motion sensor is activated; if it is, then turn reset timer
  motion_detected = digitalRead(motion_sensor);
  if (motion_detected) {
    // Reset timer when motion is detected
    startTime = millis() 
  }

  if lamp_on && millis() - tartTime >= timerDuration) {
    // turn lamp off after 30 minutes
    led_on_off();
  }

  // Update LEDs
  led_update();
}



// Controllable LED segments on/off
void led_on_off () {

  // Number of LED sections
  int numLEDs = sizeof(armedLED) / sizeof(int);


  // Toggle lamp on/off status
  if (lamp_on)
    lamp_on = 0;
  else 
    lamp_on = 1;
}


// Controls brightness of LEDs
// Three levels, 1-3
// Each segment controlled by pins 3, 5, 6, 9 (these are PWM pins)
void brightness_control (int level) {

  int duty_cycle = 0; // Duty cycle out of 255

  // Determine duty cycle
  switch (level) {

    case 1: // 33.33%%
      duty_cycle = maxBrightness * 0.33;
      break;

    case 2: // 66.67%%
      duty_cycle = maxBrightness * 0.66;
      break;

    case 3: // 100%
      duty_cycle = maxBrightness;
      break;
  }


  // Record duty cycle
  ledDutyCycle = duty_cycle;
}


// Update status of LEDs (make sure they are all on/off as designated by user)
void led_update () {

  // Number of LED sections
  int numLEDs = sizeof(armedLED) / sizeof(int);

  // Loop through all LED sections and update their status
  if (lamp_on) {

    for (int i = 0; i < numLEDs; i++) {

      if (armedLED[i] == 0)
        analogWrite(led[i], 0);
      else
        analogWrite(led[i], ledDutyCycle);
    }
  } else {  // Lamp start is off, turn off all LEDs

    for (int i = 0; i < numLEDs; i++) {

      analogWrite(led[i], 0);
    }
  }
}


// Timer feature
// If turned on, will start timer - once 1 hour is up, lamp will turn off
int TimePeriod = 1800000
int periodStartTime = 0
void timeOver () {
  currentMillis  = millis();
  if ( currentMillis - periodStartTime >= TimePeriod )
  {
    periodStartTime = currentMillis; // set new expireTime
    return true;                // more time than TimePeriod) has elapsed since last time if-condition was true
  }
  else return false;            // not expired
}
void loop ()
  if currentMillis = periodStartTime
  lamp_on