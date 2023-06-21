#include<AccelStepper.h>

const int dirPin = 6;
const int stepPin = 8;
const int triggerPin = 12;
const int echoPin = 13;
const int triggerPin2 = 9;
const int echoPin2 = 10;  
const int relayPin = 4;
unsigned char pinFlowsensor = 2;

AccelStepper stepper = AccelStepper(1, stepPin, dirPin);      
int step_valve;

int readings[5]; 
int index = 0;  
int total = 0;
int average = 0;
int duration, distance;
int tinggi = 0;
int readings2[5]; 
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
double set_point = 25.00;
double kp = 13.3;
double ki = 0.6;
double kd = 73.15;
double last_time = 0;
  
void cacahPulsa()
{
    pulsa_sensor++;
}
double pid(double error);

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  Serial.print("Setpoint = ");
  Serial.println (set_point);
  
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(triggerPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
  
  for (int i = 0; i < readings[i]; i++)
  {
      readings[i] = 0;
  }
  total = 0;

  for (int i = 0; i < readings2[i]; i++)
  {
      readings2[i] = 0;
  }
  total2 = 0;
  
  pinMode(pinFlowsensor, INPUT);
  digitalWrite(pinFlowsensor, HIGH);
  attachInterrupt(0, cacahPulsa, RISING);
  sei();
  
  waktuAktual = millis();
  waktuLoop = waktuAktual;

  stepper.setMaxSpeed(1000);      
  stepper.setAcceleration(500);   
}

void loop() {
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
  
  distance = (duration * 0.034 / 2 - 0.4393)/0.9536; ;
  distance2 = (duration2 * 0.034 / 2 - 0.4393)/0.9536; ;
  
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
  
  total2 = total2 - readings2[index2]; 
  readings2[index2] = distance2;     
  total2 = total2 + readings2[index2]; 
  index2 = index2 + 1;               
  if (index2 >= 5)
  {              
      index2 = 0; 
  }
  average2 = total2 / 5; 
  tinggi2 = 33 - average2;

//Flow Sensor Calc
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
  dt = (now - last_time)/1000.00;
  last_time = now;
  double error = set_point - tinggi;
  output_PID = pid(error);

//  Regresi Flow & Step valve
  float x = output_PID;         
  float y = (-70)*x + 5500;
  step_valve = constrain (y, 0,5400);
  stepper.moveTo(step_valve);  
  stepper.runToPosition();     

// ON OFF Pump
  if (Serial.available() > 0) 
  {
    char command = Serial.read();
    if (command == '1') {             //1 untuk On
      digitalWrite(relayPin, LOW); 
      Serial.println("PUMP ON");
    }
    else if (command == '0') {        //0 untuk Off
      digitalWrite(relayPin, HIGH);
      Serial.println("PUMP OFF");
    }
  }
 
  Serial.print("Tank 1: "); Serial2.print(tinggi); Serial.print(" cm\t");
  Serial.print("Tank 2: "); Serial2.print(tinggi2); Serial.print(" cm\t");
  Serial.print(" Flow: "); Serial2.print(literPermenit); Serial.print(" L/men\t");
  Serial.print(" PID: "); Serial2.print(output_PID); Serial.print("\t");
  Serial.print(" Valve: "); Serial2.println(step_valve);
  delay(500); 
}

double pid(double error)
{
 double proportional = error;
  integral += error*dt;
  double derivative = (error - error_sebelumnya) / dt;
  error_sebelumnya = error;
  double output_PID = (kp*proportional) + (ki*integral) + (kd*derivative);
  return output_PID;   
}
