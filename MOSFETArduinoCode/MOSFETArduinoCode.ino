/*
  MOSFET Arduino Code
  To control the a MOSFET which will control the amount of current going through it,
  thereby controlling the amount of current going through an LED
*/

#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"

VR myVR(2,3);
uint8_t records[7]; // save record
uint8_t buf[64];

// Keep track of which pin each LED associated with
int leds[] = {6, 9, 10, 11};

// Counter to keep track of if "light" was called and registered for on/off
// Even number means registered for off, odd number means registed for on
int counter = 0;

void setup() {
  myVR.begin(9600);
  for (int i = 0; i <= 2; i++) {

    if (myVR.load((uint8_t)i) >= 0)
      Serial.println("Lamp toggle function: Record " + String(i) + " recorded.");
  }

  // Set all LED pins as OUTPUT
  for (int i = 0; i < 4; i++)
    pinMode(leds[i], OUTPUT);

  Serial.begin(115200);
}

void loop() {
  int ret = myVR.recognize(buf, 50);

  // Debug
  if (ret>0 && buf[1] >= 0 && buf[1] <= 3) {

    // Increment counter
    counter++;

    if (counter % 2 == 1) {

      // All light up
      analogWrite(6, 100);
      analogWrite(9, 100);
      analogWrite(10, 100);
      analogWrite(11, 100);
      delay(1000);
      analogWrite(6, 0);
      analogWrite(9, 0);
      analogWrite(10,0);
      analogWrite(11,0);
      delay(1000);

      // // Light one at a time
      for (int i = 0; i < 4; i++) {

        analogWrite(leds[i], 25);
        delay(1000);
        analogWrite(leds[i], 0);
        delay(1000);
      }

      
      // Increase brightness from low to medium, then wait 
      // before increasing to max brightness
      for (int i = 0; i < 4; i++) {

        analogWrite(leds[i], 10);
      }
      delay(2000);
    
      for (int i = 10; i <= 100; i++) {

        analogWrite(6, i);
        analogWrite(9, i);
        analogWrite(10, i);
        analogWrite(11, i);
        delay(10);
        Serial.println(i);
      }

      delay(2000);

      for (int i = 100; i <= 255; i++) {

        analogWrite(6, i);
        analogWrite(9, i);
        analogWrite(10, i);
        analogWrite(11, i);
        delay(10);
        Serial.println(i);
      }

      delay(2000);


    } else { 

      // Off registered, turn all LEDs off
      for(int i = 0; i < 4; i++)
        analogWrite(leds[i], 0);
    }

    // for (int i = 150; i >= 0; i-=50) {


    //   analogWrite(6, i);
    //   analogWrite(9, i);
    //   analogWrite(10, i);
    //   analogWrite(11, i);
    //   delay(1500);

    //   analogWrite(6, 0);
    //   analogWrite(9, 0);
    //   analogWrite(10, 0);
    //   analogWrite(11, 0);
    //   delay(1500);
    // }

    // Brightness control
    // for (int i = 0; i <= 150; i++) {

    //   analogWrite(6, i);
    //   analogWrite(9, i);
    //   analogWrite(10, i);
    //   analogWrite(11, i);
    //   delay(50);
    // }
    // for (int i = 150; i >= 0; i--) {

    //   analogWrite(6, i);
    //   analogWrite(9, i);
    //   analogWrite(10, i);
    //   analogWrite(11, i);
    //   delay(50);
    // }

  }
}
