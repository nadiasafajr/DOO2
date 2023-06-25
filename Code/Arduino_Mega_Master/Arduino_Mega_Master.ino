void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
}

void loop() {
  // PUMP
  if (Serial.available ()>0) {
    // read user input from computer
    char command = Serial.read();
    if (command == '1')
    { //ON
      Serial3.println(command); 
    }
    else if (command == '0')
    { //OFF
      Serial3.println(command); 
    }
    else if (command == 'P')
    { //PID
      String input = Serial.readStringUntil('\n');
      Serial1.println(input); 
    }
    }
    
//SENSOR
if (Serial2.available()>0) {
    String Data = Serial2.readStringUntil('\n');
    Serial.println(Data);
  }
  
}
