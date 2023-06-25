#include <AccelStepper.h>

const int dirPin = 6;
const int stepPin = 8;
const int triggerPin = 12;
const int echoPin = 13;
const int triggerPin2 = 9;
const int echoPin2 = 10;
const int relayPin = 4;
unsigned char pinFlowsensor = 2;

AccelStepper stepper(1, stepPin, dirPin);
int step_valve;

const int numReadings = 5;
int readings[numReadings];
int index = 0;
int total = 0;
int average = 0;
int duration, distance;
int tinggi = 0;

int readings2[numReadings];
int index2 = 0;
int total2 = 0;
int average2 = 0;
int duration2, distance2;
int tinggi2 = 0;

volatile int pulsa_sensor;
float literPerjam, literPermenit;
unsigned long waktuAktual;
unsigned long waktuLoop;
double liter;

double dt;
double integral, error_sebelumnya, output_PID = 0;
double last_time = 0;
double setPoint = 0;
double kp = 0;
double ki = 0;
double kd = 0;
String input;
void cacahPulsa()
{
  pulsa_sensor++;
}

double pid(double error);

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);

  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(triggerPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);

  for (int i = 0; i < numReadings; i++)
  {
    readings[i] = 0;
    readings2[i] = 0;
  }

  pinMode(pinFlowsensor, INPUT);
  digitalWrite(pinFlowsensor, HIGH);
  attachInterrupt(0, cacahPulsa, RISING);
  sei();

  waktuAktual = millis();
  waktuLoop = waktuAktual;

  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(500);
}

void loop()
{
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  digitalWrite(triggerPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin2, LOW);

  duration = pulseIn(echoPin, HIGH);
  duration2 = pulseIn(echoPin2, HIGH);

  distance = (duration * 0.034 / 2 - 0.4393) / 0.9536;
  distance2 = (duration2 * 0.034 / 2 - 0.4393) / 0.9536;

  total = total - readings[index];
  readings[index] = distance;
  total = total + readings[index];
  index = (index + 1) % numReadings;
  average = total / numReadings;
  tinggi = 48 - average;

  total2 = total2 - readings2[index2];
  readings2[index2] = distance2;
  total2 = total2 + readings2[index2];
  index2 = (index2 + 1) % numReadings;
  average2 = total2 / numReadings;
  tinggi2 = 25 - average2;

  // Flow Sensor Calc
  waktuAktual = millis();
  if (waktuAktual >= (waktuLoop + 1000))
  {
    waktuLoop = waktuAktual;
    literPerjam = (pulsa_sensor * 60 / 7.5);
    literPermenit = literPerjam / 60;
    pulsa_sensor = 0;
  }

  // PID Calc
  double now = millis();
  dt = (now - last_time) / 1000.00;
  last_time = now;
  double error = setPoint - tinggi;
  output_PID = pid(error);

  // Regresi Flow & Step valve
  float x = output_PID;
  float y = (-70) * x + 5400;
  step_valve = constrain(y, 0, 5200);
  stepper.moveTo(step_valve);
  stepper.runToPosition();

  // ON OFF Pump
  if (Serial3.available() > 0)
  {
    char command = Serial3.read();
    if (command == '1')
    { //ON
      digitalWrite(relayPin, LOW);
    }
    else if (command == '0')
    { //OFF
      digitalWrite(relayPin, HIGH);
      stepper.moveTo(0);
      stepper.runToPosition();
    }
  }

  if (Serial1.available() > 0)
  { input = Serial1.readStringUntil('\n'); // Membaca input dari Serial1 hingga newline character
    // Memanggil fungsi pemisahan nilai
    splitValues(input);
  }

  //  SEND SENSOR DATA
  String Data = String(tinggi) + "," + String (tinggi2) + "," + String(literPermenit) + ","  + String(step_valve);
  if (Serial2.available() > 0)
  {
    Serial2.println(Data);
  }

}

double pid(double error)
{
  double proportional = error;
  integral += error * dt;
  double derivative = (error - error_sebelumnya) / dt;
  error_sebelumnya = error;
  double output_PID = (kp * proportional) + (ki * integral) + (kd * derivative);
  if (output_PID < -15) {
    output_PID = -15;
  }
  else if (output_PID > 15) {
    output_PID = 15;
  } return output_PID;
}

void splitValues(String input) {
  // Mencari indeks pemisah untuk setiap nilai
  int separatorIndex1 = input.indexOf(',');
  int separatorIndex2 = input.indexOf(',', separatorIndex1 + 1);
  int separatorIndex3 = input.indexOf(',', separatorIndex2 + 1);

  // Memisahkan nilai menggunakan substring
  String setPointString = input.substring(0, separatorIndex1);
  String kpString = input.substring(separatorIndex1 + 1, separatorIndex2);
  String kiString = input.substring(separatorIndex2 + 1, separatorIndex3);
  String kdString = input.substring(separatorIndex3 + 1);

  // Mengkonversi nilai ke tipe double
  setPoint = setPointString.toDouble();
  kp = kpString.toDouble();
  ki = kiString.toDouble();
  kd = kdString.toDouble();
}
