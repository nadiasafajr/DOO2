
//[0] Library
      #include<AccelStepper.h>
      

//[1] Deklarasi Relay (ON/OFF Pompa)
      const int relayPin1 = 4;
      const int relayPin2 = 5;

//[2] Deklarasi Motor Stepper Valve
      #define dirPin 6 // pin yang terhubung ke DIR+ motor driver
      #define stepPin 8 // pin yang terhubung ke PUL+ motor driver
      AccelStepper stepper = AccelStepper(1, stepPin, dirPin);
      float step_valve;

//[2] Deklarasi Sensor Ultra
      const int triggerPin = 12;
      const int echoPin = 13;
      int readings[5];            // array untuk menyimpan 5 pembacaan
      int index = 0;              // indeks untuk menandai pembacaan saat ini
      int total = 0;              // total dari semua pembacaan
      int average = 0;            // rata-rata dari pembacaan
      long duration, distance, tinggi;

//[3] Deklarasi Sensor Flow
      volatile int pulsa_sensor;
      float literPerjam, literPermenit;
      unsigned char pinFlowsensor = 2;
      unsigned long waktuAktual;
      unsigned long waktuLoop;
      double liter;
      void cacahPulsa()
      {
        pulsa_sensor++; 
      }

//[4] Deklarasi PID
      float Kp = 1.0;                                        //Initial Proportional Gain
      float Ki = 0.1;                                        //Initial Integral Gain
      float Kd = 0.01;                                       //Intitial Derivative Gain
      float set_point = 40;                                  //Ketinggian dinginkan = 40cm
      float error, integral_error, derivatif_error;
      float last_error = 0;
      float output_PID;
                                                       
void setup() {
//[0] Setup komunikasi serial
      Serial.begin(9600); //Baund rate Serial
      Serial2.begin(9600); //Baud rate Serial 1(Mengirim data) 
      Serial3.begin(9600); // Baud rate Serial 2 (Menerima data)
      //Serial.print("Setpoint = "); Serial.println (set_point);  //Serial awal memperlihatkan set point

//[1] Setup Relay
      pinMode(relayPin1, OUTPUT);
      pinMode(relayPin2, OUTPUT);
      digitalWrite(relayPin1, HIGH);
      digitalWrite(relayPin2, HIGH);
      
//[2] Setup sensor ultra
      pinMode(triggerPin, OUTPUT);
      pinMode(echoPin, INPUT);
      for (int i = 0; i < readings[i]; i++) {
        readings[i] = 0;}
      total = 0;
      
//[3] Setup sensor flow
      pinMode(pinFlowsensor, INPUT);
      digitalWrite(pinFlowsensor, HIGH);
      attachInterrupt(0, cacahPulsa, RISING);
      sei();
      waktuAktual = millis();
      waktuLoop = waktuAktual;
      
//[4] Setup Motor Stepper Valve
    //stepper.setMaxSpeed(1000);      // atur kecepatan, dapat docoba dirubah untuk latihan
    //stepper.setAcceleration(500);   // nilai akselerasi / percepatan, dapat docoba dirubah untuk latihan
}

void loop()
{ 
//[1] ON/OFF pompa-->serial monitor-->relay
      if (Serial.available() > 0) 
      {
        char command = Serial.read();
        if (command == '1') {             
          digitalWrite(relayPin1, LOW); // PUMP ON
        }
        else if (command == '0') {      
          digitalWrite(relayPin1, HIGH); // PUMP OFF
        }
       }
       
             
//[2] Perhitungan Sensor Ultra
      digitalWrite(triggerPin, LOW);
      delayMicroseconds(2);
      digitalWrite(triggerPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(triggerPin, LOW);
      duration = pulseIn(echoPin, HIGH);
      distance = duration * 0.034 / 2;
      total = total - readings[index]; // mengurangi pembacaan lama
      readings[index] = distance;      // menambahkan pembacaan baru
      total = total + readings[index]; // Menotalkan pembacaan baru
      index = index + 1;               // mengubah indeks ke pembacaan berikutnya
                          
      if (index >= 5)
      {                         // jika sudah mencapai batas pembacaan
        index = 0;              // indeks max diubah jadi 0
      }         
      average = total / 5;      // menghitung rata-rata
      tinggi = 50 - average;    // tinggi air di tangki

//[3] Perhitungan Sensor Flow
      waktuAktual = millis();
      if(waktuAktual >= (waktuLoop + 1000))
      {
          waktuLoop = waktuAktual;
          literPerjam = (pulsa_sensor*60/7.5);
          literPermenit = literPerjam / 60;
          pulsa_sensor = 0;
      }
      
//[4] Perhitungan PID
      error = set_point - tinggi;
      integral_error += error;              //atau: error*0.1 (time sampling)
      derivatif_error = error-last_error;   //atau: (error-last_error)/0.1
      last_error = error;                   //utk perhitungan berikutnya
      
      output_PID = Kp*error + Ki*integral_error + Kd*derivatif_error;
      //note: diperkirakan output PID adlh flow (Qin), yg menjadi input plant
      
      if (output_PID > 0.0003){        //Qin tdk boleh > 18L/men
        output_PID = 0.0003;
      }
      else if (output_PID < 0){  //Qin tdk boleh < 0L/men
        output_PID = 0;
      }
      else{
        output_PID = output_PID;
      }
      
//[5] Regresi Flow & Step valve
      float x = output_PID;
      float y =  200*x + 3000;
      step_valve = y;

//[6] PID control bukaan valve
      delay(100);                   //delay output_PID dipakai
      stepper.moveTo(step_valve);   //beri target 360*
      stepper.runToPosition();      //jalankan
      //if(stepper.runToPosition()){
            digitalWrite(relayPin2, LOW); //Fan ON
      
      //analogWrite(3, output_PID); 

//[7] Menampilkan tinggi dan flow air
            Serial.print("Waterlevel: ");
            Serial.print(tinggi);
            Serial.print(",");
            Serial.print(" Flow: ");
            Serial.print(literPermenit);
            Serial.print(",");
            Serial.print(" PID: ");
            Serial.println(output_PID);
            delay(300); 
            //seluruh kejadian di void loop delay selama 300ms
}