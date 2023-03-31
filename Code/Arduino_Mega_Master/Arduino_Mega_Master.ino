void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == '1') {             
      Serial.println("Pump On");
      Serial3.println('1'); // send "1" as a string
    }
    else if (command == '0') { 
      Serial.println("Pump Off");
      Serial3.println('0'); // send "0" as a string     
    }
  }
}
