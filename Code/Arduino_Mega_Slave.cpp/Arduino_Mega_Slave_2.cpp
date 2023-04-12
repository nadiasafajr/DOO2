#define dirPin 6               // pin yang terhubung ke DIR+ motor driver
#define stepPin 8              // pin yang terhubung ke PUL+ motor driver
#define stepsPerRevolution 800 // sesuaikan dengan settingan SW1-SW3 pada modul motor driver
float step_valve;
int currentPosition = 0;
int direction = 0;

const int relayPin1 = 4;
const int relayPin2 = 5;

const int triggerPin = 12;
const int echoPin = 13;

volatile int pulsa_sensor; 
float literPerjam, literPermenit;
unsigned char pinFlowsensor = 2;
unsigned long waktuAktual;
unsigned long waktuLoop;
double liter;

int readings[5]; // array untuk menyimpan 5 pembacaan
int index = 0;   // indeks untuk menandai pembacaan saat ini
int total = 0;   
int average = 0; 
char userInput;
int data = 0;

float Kp = 1.0;      
float Ki = 0.1;       
float Kd = 0.01;      
float set_point = 40; 
float error, integral_error, derivatif_error;
float last_error = 0;
float output_PID;

void cacahPulsa()
{
    pulsa_sensor++;
}

void setup()
{
    Serial.begin(9600);
    Serial2.begin(9600);
    Serial3.begin(9600);

    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    currentPosition = 0;
    randomSeed(analogRead(0));

    pinMode(triggerPin, OUTPUT);
    pinMode(echoPin, INPUT);

    pinMode(relayPin1, OUTPUT);
    pinMode(relayPin2, OUTPUT);

    digitalWrite(relayPin1, LOW);
    digitalWrite(relayPin2, LOW);

    pinMode(pinFlowsensor, INPUT);
    digitalWrite(pinFlowsensor, HIGH);
    attachInterrupt(0, cacahPulsa, RISING);
    sei();
    waktuAktual = millis();
    waktuLoop = waktuAktual;

    for (int i = 0; i < readings[i]; i++)
    {
        readings[i] = 0;
    }
    total = 0;
}

void loop()
{
    long duration, distance1, distance, tinggi;
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;
    distance = (distance1 - 0.4393) / 0.9536;
    total = total - readings[index]; // mengurangi pembacaan lama
    readings[index] = distance;      // menambahkan pembacaan baru
    total = total + readings[index]; // menambahkan pembacaan baru
    index = index + 1;               // mengubah indeks ke pembacaan berikutnya

    if (index >= 5)
    {              // jika sudah mencapai batas pembacaan
        index = 0; // kembali ke indeks pertama
    }
    average = total / 5; // Menghitung rata-rata sensor ultrasonic
    tinggi = 49 - average;

    waktuAktual = millis(); // waterflow
    if (waktuAktual >= (waktuLoop + 1000))
    {
        waktuLoop = waktuAktual;
        literPerjam = (pulsa_sensor * 60 / 5.5);
        literPermenit = literPerjam / 60;
        pulsa_sensor = 0;
    }
    delay(300);

    error = set_point - tinggi;
    integral_error += error;              // atau: error*0.1 (time sampling)
    derivatif_error = error - last_error; // atau: (error-last_error)/0.1
    last_error = error;                   // utk perhitungan berikutnya

    output_PID = Kp * error + Ki * integral_error + Kd * derivatif_error;
    // note: diperkirakan output PID adlh flow (Qin), yg menjadi input plant

    if (output_PID > 0.0003)
    { // Qin tdk boleh > 18L/men
        output_PID = 0.0003;
    }
    else if (output_PID < 0)
    { // Qin tdk boleh < 0L/men
        output_PID = 0;
    }
    else
    {
        output_PID = output_PID;
    }

    float x = output_PID;
    float y = 200 * x + 3000;
    step_valve = y;

    if (step_valve > currentPosition)
    {
        direction = 1;
    }
    else if (step_valve < currentPosition)
    {
        direction = -1;
    }

    // Move the motor one step in the appropriate direction
    for (int i = 0; i < step_valve; i++)
    {
        digitalWrite(dirPin, direction);
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(500);

        // Update the current position
        currentPosition += direction;
    }

    if (Serial3.available())
    {
        char command = Serial3.read();

        if (command == '1')
        {
            digitalWrite(relayPin2, LOW); // Pump On
        }
        else if (command == '0')
        {
            digitalWrite(relayPin2, HIGH); // Pump Off
        }
    }
    // Sent Data to Arduino Master
    Serial2.print("Waterlevel: ");
    Serial2.print(tinggi);
    Serial2.print(",");
    Serial2.print(" Flow: ");
    Serial2.print(literPermenit);
    Serial2.print(",");
    Serial2.print(" PID: ");
    Serial2.println(output_PID);
    delay(300);
}
