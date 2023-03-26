
#include <SoftwareSerial.h>

SoftwareSerial serial1(2, 3); // RX, TX
SoftwareSerial serial2(4, 5); // RX, TX


void setup() {
  serial1.begin(9600);
  serial2.begin(9600);
 
}

void loop() {
  if (serial1.available()>=2) {
    int tinggi = Serial1.read(); // Baca nilai ultrasonic dari serial
    int literPermenit = Serial1.read(); // Baca nilai flow dari serial
  Serial.print("Waterlevel: "); Serial.print(tinggi); Serial.print(" cm\t");
  Serial.print(" Flow: "); Serial.print(literPermenit); Serial.println(" L/men");
  }

  if (serial.available()) {
    int data = serial2.read();
    // do something with the received data
  }

  if (serial3.available()) {
    int data = serial3.read();
    // do something with the received data
  }
}
