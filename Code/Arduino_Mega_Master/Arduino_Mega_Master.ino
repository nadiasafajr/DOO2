void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    // read user input from computer
    char command = Serial.read();

    // send command to slave using Serial2
    Serial3.write(command);
  }

if (Serial2.available()) {
    // read and print distance data from ultrasonic sensor on slave
    long distance = Serial2.parseInt();
    Serial.print("Distance: ");
    Serial.println(tinggi);

    // read and print flow rate data from flow sensor on slave
    int flowRate = Serial2.parseInt();
    Serial.print("Flow rate: ");
    Serial.println(literPermenit);
  }
delay(300);
}
