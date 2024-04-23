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
 *  - "High" - Brightness up  (record 5)
 *  - "Low"    - Brightness down (record 6)
 *
 * Timer Aspect:
 *  - User will have option to turn on/off the timer, which sets lamp to turn off by itself after a set period of time
 *  - Timer will reset whenever the motion sensor is triggered
 *  - If lamp is off, it will turn on again after motion sensor is triggered
 *
 * Orientation Commands:
 *  - Quadrant I ("one") - record 1
 *  - Quadrant II ("two") - record 2
 *  - Quadrant III ("three") - record 3
 *  - Quadrant IV ("four") - record 4
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
int armedLED[4] = {1, 1, 1, 1};


// List of brightnesses
int brightness_list[5] = {50, 100, 150, 200, 250}; // Default max brightness
int brightness_selector = 2;

// Duty cycle of LEDs when turned on
int maxBrightness = brightness_list[4];
int ledDutyCycle = brightness_list[2];


// Motion sensor pins
int motion_sensor = 4;
int motion_detected = 0;

// timer ints
long startTime = 0;
long timerDuration = 15 * 60 * 1000; // 15 minutes in milliseconds


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
    
  // Voice recognition 
  if(myVR.clear() == 0){
    Serial.println("Recognizer cleared.");
  }else{
    Serial.println("Not find VoiceRecognitionModule.");
    Serial.println("Please check connection and restart Arduino.");
    while(1);
  }
  

  // Load recordings for lamp on/off toggle (record 0)
  if (myVR.load((uint8_t)0) >= 0)
    Serial.println("Lamp toggle function: Record " + String(0) + " recorded.");


  // Load orientation commands (records 12 to 23)
  for (int i = 1; i <= 4; i++) {

    if (myVR.load((uint8_t)i) >= 0)
      Serial.println("Orientation Command: Record " + String(i) + " recorded.");
  }

  // Load brightness commands (records 3 to 11)
  for (int i = 5; i <= 6; i++) {

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

    if (buf[1] == 0) {           // Check for on/off command

      // Turn off all controllable segments
      led_on_off();
    } else if (buf[1] == 5 || buf[1] == 6) {   // Check for brightness command

      // Swtiches brightness based on input
      if (buf[1] == 5) {                        // Brightness increase command

        brightness_control(1);
      } else if (buf[1] == 6) {                 // Brightness decrease command

        brightness_control(0);
      }
        
    } else if (buf[1] >= 1 && buf[1] <= 4) {  // Check for orientation command
    
      int selected_LED = buf[1]-1;
      
      // Don't execute LED off command if there is only one LED on
      int lights_on = 0;
      for (int i = 0; i <= 3; i++) {

        if (armedLED[i] == 1)
          lights_on++;
      }

      if (armedLED[selected_LED] == 1 && lights_on != 1)
        armedLED[selected_LED] = 0;
      else
        armedLED[selected_LED] = 1;
    } else {

      Serial.println("Unexpected error: no command found");
    }

    /** voice recognized */
    printVR(buf);

    // reset timer whenever voice command is called
    startTime = millis(); 
  }


  // Determine if motion sensor is activated; if it is, then turn reset timer
  motion_detected = digitalRead(motion_sensor);
  if (motion_detected) {
    // Reset timer when motion is detected, turn on lamp
    startTime = millis();
    lamp_on = 1;
  }

  if (lamp_on && (millis() - startTime) >= timerDuration) {
    // turn lamp off after 30 minutes
    lamp_on = 0;
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
// direction dictates whether to make brighter (1) or dimmer (0)
void brightness_control (int direction) {

  int max_bright = sizeof(brightness_list) / sizeof(int) - 1;   // Max brightness option (number of brightness levels - 1 due to array)

  if (direction) {                                            // Brighter command

    if (brightness_selector != max_bright)
      brightness_selector++;
  } else {                                                    // Dimmer command

    if (brightness_selector != 0)
      brightness_selector--;
  }

  // Set brightness
  ledDutyCycle = brightness_list[brightness_selector];
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
