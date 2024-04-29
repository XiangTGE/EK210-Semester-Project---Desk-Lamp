/*
  MOSFET Arduino Code
  To control the a MOSFET which will control the amount of current going through it,
  thereby controlling the amount of current going through an LED
*/


void setup() {

  pinMode(6, OUTPUT);
}

void loop() {

  // All light up
  analogWrite(6, 100);
}
