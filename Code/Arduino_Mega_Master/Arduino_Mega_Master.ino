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
      Serial3.print('1'); // send "1" as a string
    }
    else if (command == '0') { 
      Serial3.print('0'); // send "0" as a string     
    }
  }

if (Serial2.available() > 0) {
    String data = Serial2.readStringUntil('\n');
    int separatorIndex = data.indexOf(',');
    if (separatorIndex != -1) {
      int tinggi = data.substring(1, separatorIndex).toInt();
      int literPermenit = data.substring(separatorIndex + 1).toInt();
      Serial.print("Water level: ");
      Serial.println(tinggi);
      Serial.print("Flow: ");
      Serial.println(literPermenit);
    }
  }
delay(300);
}
