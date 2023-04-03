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

    // send command to slave using Serial3
    Serial3.print(command);
  }

if (Serial2.available()) {
    //int tinggi = Serial2.parseInt();
    //Serial.print("Distance: ");
    //Serial.print(tinggi);

    // read and print flow rate data from flow sensor on slave
    //int literPermenit = Serial2.parseInt();
    //Serial.print("Flow rate: ");
    //Serial.println(literPermenit);

    String data = Serial2.readStringUntil('\n');
    
    // Pisahkan data menjadi nilai jarak dan aliran
    int separatorIndex = data.indexOf(',');
    int tinggi; //= data.substring(1,separatorIndex).toInt();
    int literPermenit = data.substring(separatorIndex + 1).toInt();
    Serial.print(tinggi);
    Serial.println(literPermenit);

  }
}
