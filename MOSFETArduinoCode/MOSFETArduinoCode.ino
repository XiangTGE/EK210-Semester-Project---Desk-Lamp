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

void setup() {
  myVR.begin(9600);
  for (int i = 0; i <= 2; i++) {

    if (myVR.load((uint8_t)i) >= 0)
      Serial.println("Lamp toggle function: Record " + String(i) + " recorded.");
  }

  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  int ret = myVR.recognize(buf, 50);

  // Debug
  if (ret>0 && buf[1] >= 0 && buf[1] <= 3) {

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
    // analogWrite(6, 25);
    // delay(1000);
    // analogWrite(6, 0);
    // delay(1000);

    // analogWrite(9, 25);
    // delay(1000);
    // analogWrite(9, 0);
    // delay(1000);

    // analogWrite(10, 25);
    // delay(1000);
    // analogWrite(10, 0);
    // delay(1000);

    // analogWrite(11, 25);
    // delay(1000);
    // analogWrite(11, 0);
    // delay(1000);

    
    // for (int i = 0; i <= 150; i+=50) {

    //   analogWrite(6, i);
    //   analogWrite(9, i);
    //   analogWrite(10, i);
    //   analogWrite(11, i);
    //   delay(1000);

    //   analogWrite(6, 0);
    //   analogWrite(9, 0);
    //   analogWrite(10, 0);
    //   analogWrite(11, 0);
    //   delay(1000);
    // }

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
