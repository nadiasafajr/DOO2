#include <SoftwareSerial.h>
#include<AccelStepper.h>
#define dirPin 6 // pin yang terhubung ke DIR+ motor driver
#define stepPin 8 // pin yang terhubung ke PUL+ motor driver
AccelStepper stepper = AccelStepper(1, stepPin, dirPin);
SoftwareSerial mySerial(10,11);
const int relayPin = 4;
const int triggerPin = 12;
const int echoPin = 13;
const int buzzer = 9;
volatile int pulsa_sensor;//Cekflow
float literPerjam, literPermenit;
unsigned char pinFlowsensor = 2;
unsigned long waktuAktual;
unsigned long waktuLoop;
double liter;


int readings[5]; // array untuk menyimpan 5 pembacaan
int index = 0; // indeks untuk menandai pembacaan saat ini
int total = 0; // total dari semua pembacaan
int average = 0; // rata-rata dari pembacaan
char userInput;
int data = 0;

void cacahPulsa()
{
  pulsa_sensor++;
}

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  stepper.setMaxSpeed(1000); // atur kecepatan, dapat docoba dirubah untuk latihan
  stepper.setAcceleration(500); // nilai akselerasi / percepatan, dapat docoba dirubah untuk latihan
  pinMode(buzzer,OUTPUT);
  tone(buzzer,2500);delay(1500);noTone(buzzer);
  randomSeed(analogRead(0));
  //myStepper.setSpeed(60); // set kecepatan motor stepper
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  for (int i = 0; i < readings[i]; i++) {
    readings[i] = 0;
  }
  total = 0;
  pinMode(pinFlowsensor, INPUT);
  digitalWrite(pinFlowsensor, HIGH);
  attachInterrupt(0, cacahPulsa, RISING);
  sei();
  waktuAktual = millis();
  waktuLoop = waktuAktual;
}

void loop() {

  

  long duration,distance,tinggi;
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
  tinggi = 49 - average;


  waktuAktual = millis(); //waterflow
  if (waktuAktual >= (waktuLoop + 1000))
  {
    waktuLoop = waktuAktual;
    literPerjam = (pulsa_sensor * 60 / 7.5);
    literPermenit = literPerjam / 60;
    pulsa_sensor = 0;

  }
  //Serial.print("Waterlevel: "); 
  Serial2.println(tinggi); 
  Serial.println(tinggi); 
  //Serial.print(" cm\t");
//  Serial.print(",");
  //Serial.print(" Flow: "); 
//  Serial2.println(literPermenit); 
  //Serial.println(" L/men");
  
  delay(300);
  //mengaktifkan pompa dengan serial monitor
  if (Serial3.available()) {
    char command = Serial3.read();

    if (command == '1') {
      digitalWrite(relayPin, LOW); // Pump On
    }
    else if (command == '0') {
      digitalWrite(relayPin, HIGH); // Pump Off
    }
    
  }
    }
