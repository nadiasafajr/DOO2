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
double total = 0;
double average = 0;
double duration, distance;
double tinggi = 0;

int readings2[numReadings];
int index2 = 0;
double total2 = 0;
double average2 = 0;
double duration2, distance2;
double tinggi2 = 0;

volatile int pulsa_sensor;
float literPerjam, literPermenit;
unsigned long waktuAktual;
unsigned long waktuLoop;
double liter;

double dt;
double integral, error_sebelumnya, output_PID = 0;
double last_time = 0;
double setPoint ;
double kp ;
double ki ;
double kd ;
String input;
String separator = ",";
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
  step_valve = 0;
}

void loop()
{
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.034 / 2 - 0.4393) / 0.9536;
  total = total - readings[index];
  readings[index] = distance;
  total = total + readings[index];
  index = index + 1;
  if (index >= 5)
  {
    index = 0;
  }
  average = total / 5;
  tinggi = 48 - average;
  constrain(tinggi, 0, 48);

  digitalWrite(triggerPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distance2 = (duration2 * 0.034 / 2 - 0.4393) / 0.9536;
  total2 = total2 - readings2[index2];
  readings2[index2] = distance2;
  total2 = total2 + readings2[index2];
  index2 = index2 + 1;
  if (index2 >= 5)
  { // jika sudah mencapai batas pembacaan
    index2 = 0; // kembali ke indeks pertama
  }
  average2 = total2 / 5;
  tinggi2 = 33 - average2;
  constrain(tinggi, 0, 33);
  // Flow Sensor Calc
  waktuAktual = millis();
  if (waktuAktual >= (waktuLoop + 1000))
  {
    waktuLoop = waktuAktual;
    literPerjam = (pulsa_sensor * 60 / 7.5);
    literPermenit = literPerjam / 60;
    pulsa_sensor = 0;
  }

  //  SEND SENSOR DATA
  String Data = String(tinggi) + "," + String (tinggi2) + "," + String(literPermenit) + ","  + String(step_valve);
  Serial2.println(Data);
  delay(100);
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
    else if (command == '4')
    { step_valve += 800;
      Serial.println("Menambah tutupan ke 800");
    }
    else if (command == '5')
    { step_valve -= 800;
      Serial.println("Menambah bukaan ke -800");
    }
  }


  if (Serial1.available() > 0)
  {
    char command = Serial1.read();
    if (command == '2') {
      tangki_1();
    }
    else if (command == '3') {
      tangki_2();
    }
  }
  if (Serial2.available() > 0)
  { input = Serial2.readStringUntil('\n'); // Membaca input dari Serial1 hingga newline character
    // Memanggil fungsi pemisahan nilai
    splitValues(input);
    Serial.print(kp);
    Serial.print(ki);
    Serial.print(kd);
    Serial.print(setPoint);
    Serial.print(step_valve);
  }
}
void splitValues(String input) {
  // Mencari indeks pemisah untuk setiap nilai
  int separatorIndex1 = input.indexOf(separator);
  int separatorIndex2 = input.indexOf(separator, separatorIndex1 + 1);
  int separatorIndex3 = input.indexOf(separator, separatorIndex2 + 1);
  int separatorIndex4 = input.indexOf(separator, separatorIndex3 + 1);

  // Memisahkan nilai menggunakan substring
  String kpString = input.substring(1, separatorIndex1);
  String kiString = input.substring(separatorIndex1 + 1, separatorIndex2);
  String kdString = input.substring(separatorIndex2 + 1, separatorIndex3);
  String setPointString = input.substring(separatorIndex3 + 1, separatorIndex4);
  String valveString = input.substring(separatorIndex4 + 1);

  // Mengkonversi nilai ke tipe double dan int
  kp = kpString.toDouble();
  ki = kiString.toDouble();
  kd = kdString.toDouble();
  setPoint = setPointString.toDouble();
  step_valve = valveString.toInt();
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
void tangki_1() {
  for (;;) {
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
  }
}

void tangki_2() {
  for (;;) {
    double now = millis();
    dt = (now - last_time) / 1000.00;
    last_time = now;
    double error = setPoint - tinggi2;
    output_PID = pid(error);

    // Regresi Flow & Step valve
    float x = output_PID;
    float y = (-70) * x + 5400;
    step_valve = constrain(y, 0, 5200);
    stepper.moveTo(step_valve);
    stepper.runToPosition();
  }
}
