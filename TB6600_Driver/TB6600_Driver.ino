 //Define stepper motor connections and steps per revolution:
#define dirPin 2
#define stepPin 3
#define stepsPerRevolution 800 // 800 STEP PER 1 REVOLUTION
int j = 0;
void setup() {
  // Declare pins as output:
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
}

void loop(){

while (j<5){
  // Set the spinning direction clockwise:
  digitalWrite(dirPin, HIGH);  // HIGH = CLOCKWISE, LOW = CC

  // Spin the stepper motor 1 revolution slowly:
  for (int i = 0; i < stepsPerRevolution; i++) {
    // These four lines result in 1 step:
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(2000);
  }
  delay(1000);
  j=j+1;



   
}
}
