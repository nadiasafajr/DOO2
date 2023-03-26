#include <SoftwareSerial.h>
#include<Stepper.h>
const int relayPin = 4;
const int triggerPin = 12;
const int echoPin = 13;
byte sensorInt = 0;
byte flowsensor = 2;
float konstanta = 4.5;

const int stepsPerRevolution = 800; // jumlah step per revolusi motor stepper
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11); // inisialisasi motor stepper

int readings[5]; // array untuk menyimpan 5 pembacaan
int index = 0; // indeks untuk menandai pembacaan saat ini
int total = 0; // total dari semua pembacaan
int average = 0; // rata-rata dari pembacaan
int data = 0;
char userInput;

volatile byte pulseCount;
float debit;
unsigned int flowmlt;
unsigned long totalmlt;
unsigned long oldTime;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  //myStepper.setSpeed(60); // set kecepatan motor stepper
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
  volatile byte pulseCount;
  float debit;
  unsigned int flowmlt;
  unsigned long totalmlt;
  unsigned long oldTime;
  attachInterrupt(sensorInt, pulseCounter, FALLING);
  for (int i = 0; i < readings[i]; i++) {
    readings[i] = 0;
  }
  total = 0;
}

void loop() {

  //myStepper.step(400); // gerakkan motor stepper sebanyak 400 step
  //delay(1000); // tunggu selama 1 detik
  //myStepper.step(-400); // gerakkan motor stepper sebanyak -400 step (berlawanan arah)
  //Code untuk pembacaan sensor ultrasonic

  long duration, distance, tinggi;
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  total = total - readings[index]; // mengurangi pembacaan lama
  readings[index] = distance; // menambahkan pembacaan baru
  total = total + readings[index]; // menambahkan pembacaan baru
  index = index + 1; // mengubah indeks ke pembacaan berikutnya

  if (index >= 5) { // jika sudah mencapai batas pembacaan
    index = 0; // kembali ke indeks pertama
  }
  average = total / 5; // menghitung rata-rata
  tinggi = 50 - average; //Nilai Sensor

  //Serial.print("Waterlevel: ");Serial.print(tinggi);Serial.println(" cm");
  if ((millis() - oldTime) > 1000)
  {
    detachInterrupt(sensorInt);
    debit = ((1000.0 / (millis() - oldTime)) * pulseCount) / konstanta;
    oldTime = millis();
    flowmlt = (debit / 60) * 1000;
    totalmlt += flowmlt;

    unsigned int frac;

    Serial.print("Debit air: ");
    Serial.print(int(debit)\n);
    Serial.print("L/min");

    pulseCount = 0;
    attachInterrupt(sensorInt, pulseCounter, FALLING);
  }
  delay(500);

  //mengaktifkan pompa dengan serial monitor
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == '1') {
      digitalWrite(relayPin, LOW);
      Serial.println("PUMP ON");
    }
    else if (command == '0') {
      digitalWrite(relayPin, HIGH);
      Serial.println("PUMP OFF");
    }
    
    userInput =  Serial.read();
    if (userInput == 'g') {                // if we get expected value

      data = tinggi;    // read the input pin
      Serial.println(data);
    }
  }
}
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}
