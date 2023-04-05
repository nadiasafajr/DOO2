void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
}

void loop() {
  // put your main code here, to run   repeatedly:
  if (Serial.available ()>0) {
    // read user input from computer
    char command = Serial.read();

    // send command to slave using Serial3
    Serial3.print(command);
  }

if (Serial2.available()>0) {
    String data = Serial.readStringUntil('\n');
    
    // Split data into water level, flow rate and PID values
    int separatorIndex1 = data.indexOf(',');
    int separatorIndex2 = data.indexOf(',', separatorIndex1 + 1);
    int tinggi = data.substring(12, separatorIndex1).toInt();
    int literPermenit = data.substring(separatorIndex1 + 8, separatorIndex2).toInt();
    int output_PID = data.substring(separatorIndex2 + 6).toInt();
    
    // Print water level, flow rate and PID values
    Serial.print("Water level: ");
    Serial.println(tinggi);
    Serial.print("Flow rate: ");
    Serial.println(literPermenit);
    Serial.print("PID: ");
    Serial.println(output_PID);
  }
}