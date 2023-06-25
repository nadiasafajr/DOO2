/* PROGRAM INI MENGGUNAKAN FUNGSI WAKTU (dt)
 *  
 * https://www.youtube.com/watch?v=RZW1PsfgVEI
 */
//[0] Library
      #include<AccelStepper.h>
      
//[1] Deklarasi Relay (ON/OFF Pompa)
      const int relayPin = 4;
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
      double dt, last_time;
      double integral, error_sebelumnya, output_PID = 0;
      double kp, ki, kd;
      double set_point = 8.00;                           //Ketinggian dinginkan = 30cm
                                                       
void setup() {
//[0] Setup komunikasi serial
      Serial.begin(9600);                                 //Baund rate Serial

//[1] Setup Relay
      pinMode(relayPin2, OUTPUT);
      digitalWrite(relayPin2, LOW);

      pinMode(relayPin, OUTPUT);
      digitalWrite(relayPin, LOW);
      
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

//[5] Setup PID
      kp = 20;
      ki = 9;
      kd = 0.01;
      last_time = 0;
      Serial.print("Setpoint = "); Serial.println (set_point);  //Serial awal memperlihatkan set point
}

void loop()
{ 
//[1] ON/OFF pompa-->serial monitor-->relay
    //  if (Serial.available() > 0) 
     // {
        //char command = Serial.read();
        //if (command == '1') {             //1 untuk On
         // digitalWrite(relayPin2, LOW); 
         // Serial.println("PUMP ON");
        //l}
        //else if (command == '0') {        //0 untuk Off
          //digitalWrite(relayPin2, HIGH);
          //Serial.println("PUMP OFF");
        //}
       //}
       
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
      double now = millis();
      dt = (now - last_time)/1000.00;
      last_time = now;

      double error = set_point - tinggi;
      output_PID = pid(error);
      
//[5] Regresi Flow & Step valve
      // float x = literPermenit;   //output PID yg dibaca oleh sensor flow
      float x = output_PID;         //output PID yg berupa flow
      float y = 900*x + 3000;
      step_valve = y;

//[6] PID control bukaan valve
      //delay(100);                 //delay menunggu bukaan valve sebelumnya mencapai posisi yg dituju
      stepper.moveTo(step_valve);   //beri target 360*
      stepper.runToPosition();      //jalankan
      
      //analogWrite(3, output_PID);  //kalau pake motor DC kuning

//[7] Menampilkan tinggi dan flow air
      Serial.print("Waterlevel: "); Serial.print(tinggi); Serial.print(" cm\t");
      Serial.print(" Flow: "); Serial.print(literPermenit); Serial.print(" L/men\t");
      Serial.print(" PID: "); Serial.println(output_PID);
      delay(300);   //seluruh kejadian di void loop delay selama 300ms
}

//Fungsi khusus PID
double pid(double error)
{
  double proportional = error;
  integral += error*dt;
  double derivative = (error - error_sebelumnya) / dt;
  error_sebelumnya = error;
  double output_PID = (kp*proportional) + (ki*integral) + (kd*derivative);
  return output_PID;

  if (output_PID > 0.0003){        //Qin tdk boleh > 18L/men
    output_PID = 0.0003;
    }
   else if (output_PID < 0){  //Qin tdk boleh < 0L/men
    output_PID = 0;
    }
   else{
    output_PID = output_PID;
    }   
}

      
