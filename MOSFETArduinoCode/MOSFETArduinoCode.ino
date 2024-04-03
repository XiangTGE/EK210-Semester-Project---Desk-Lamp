/*
  MOSFET Arduino Code
  To control the a MOSFET which will control the amount of current going through it,
  thereby controlling the amount of current going through an LED
*/

void setup() {
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // digitalWrite(3, LOW);

  // delay(10);

  // digitalWrite(3, HIGH);

  // delay(1);

  // Set up a duty cycle
  //analogWrite(3, 255);

  // Debug
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

  // // All light up
  // analogWrite(6, 25);
  // analogWrite(9, 25);
  // analogWrite(10, 25);
  // analogWrite(11, 25);
  // delay(5000);
  // analogWrite(6, 0);
  // analogWrite(9, 0);
  // analogWrite(10,0);
  // analogWrite(11,0);
  // delay(2000);
  
  // Brightness control
  for (int i = 10; i <= 255; i+=50) {

    analogWrite(6, i);
    analogWrite(9, i);
    analogWrite(10, i);
    analogWrite(11, i);
    delay(1000);

    analogWrite(6, 0);
    analogWrite(9, 0);
    analogWrite(10, 0);
    analogWrite(11, 0);
    delay(1000);
  }

  for (int i = 255; i >= 0; i-=50) {

    analogWrite(6, i);
    analogWrite(9, i);
    analogWrite(10, i);
    analogWrite(11, i);
    delay(1000);

    analogWrite(6, 0);
    analogWrite(9, 0);
    analogWrite(10, 0);
    analogWrite(11, 0);
    delay(1000);
  }
}