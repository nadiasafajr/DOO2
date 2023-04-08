const int sensorPin = 2; // attach flow meter to digital pin 2

volatile int pulseCount; // count pulses from flow meter
float flowRate;          // calculated flow rate
unsigned int flowMilliLitres;  // total accumulated flow

unsigned long oldTime;

void setup() {
  Serial.begin(9600);   // initialize serial communication
  attachInterrupt(0, pulseCounter, FALLING); // configure interrupt
  pulseCount = 0;       // reset pulse count
  flowRate = 0.0;       // initialize flow rate
  flowMilliLitres = 0;  // initialize total accumulated flow
  oldTime = 0;
}

void loop() {
  unsigned long currentTime = millis();

  if(currentTime - oldTime > 1000)  // update every 1 second
  {
    detachInterrupt(0);             // disable interrupt
    flowRate = pulseCount / 5.5;    // convert pulse count to flow rate (L/min)
    flowMilliLitres += (flowRate / 60) * 1000; // calculate total accumulated flow (mL)
    pulseCount = 0;                 // reset pulse count
    oldTime = currentTime;          // save current time
    attachInterrupt(0, pulseCounter, FALLING); // enable interrupt
  }

  Serial.print("Flow rate: ");
  Serial.print(flowRate);
  Serial.print(" L/min");

  Serial.print("  Total flow: ");
  Serial.print(flowMilliLitres);
  Serial.println(" mL");

  delay(500);   // add delay to avoid spamming serial monitor
}

void pulseCounter() {
  pulseCount++;
}
