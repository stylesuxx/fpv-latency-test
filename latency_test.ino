// Setup pins to your liking
const int photoPin = A0;
const int ledPin = 13;

const unsigned int measurementCount = 25;
unsigned long measurements[measurementCount];

unsigned int current;
unsigned int command;

unsigned long threshold = 10;
unsigned long min = 0;
unsigned long max = 0;
unsigned long average = 0;

unsigned long start;
unsigned long end;
unsigned int i;

const unsigned int randomDelayMin = 1000;
const unsigned int randomDelayMax = 2000;

bool running = false;

void calibration() {
  digitalWrite(ledPin, 0);
  delay(500);
  min = analogRead(photoPin);

  digitalWrite(ledPin, 1);
  delay(500);
  max = analogRead(photoPin);
  digitalWrite(ledPin, 0);

  /* Threshold is the minimum value increased by 10.
   *  
   * This might need to be adapted to the exact used LED. The lower the threshold is,
   * the less time is "wasted" for the LED to be bright enough. We are talking about
   * potentially 100us just  by increasing the threshold by 10.
   */
  threshold = min + 10;

  Serial.print("Min: ");
  Serial.print(min);
  Serial.print(" Max: ");
  Serial.print(max);
  Serial.print(" Threshold: ");
  Serial.println(threshold);
}

void printResults() {
  average = 0;
  max = 0;  
  min = measurements[0];

  Serial.println("##### START #####");
  for(i = 0; i < measurementCount; i += 1) {
    if(measurements[i] > max) {
      max = measurements[i];
    }

    if(measurements[i] < min) {
      min = measurements[i];
    }
    
    average += measurements[i];
    if(i < 10) {
      Serial.print(0);
    }
    Serial.print(i);
    Serial.print(":\t");
    Serial.print(measurements[i]);
    Serial.print("us\t(");
    Serial.print(measurements[i] / 1000);
    Serial.println("ms)");
  }
  Serial.println("#################");
  Serial.print("Avg:\t");
  Serial.print(average / measurementCount);
  Serial.print("us\t(");
  Serial.print(average / measurementCount / 1000);
  Serial.println("ms)");
  Serial.print("Min:\t");
  Serial.print(min);
  Serial.print("us\t(");
  Serial.print(min / 1000);
  Serial.println("ms)");
  Serial.print("Max:\t");
  Serial.print(max);
  Serial.print("us\t(");
  Serial.print(max / 1000);
  Serial.print("ms)");
  Serial.println();
  
  Serial.println("#####  END  #####");
}

void setup() {
  pinMode(photoPin, INPUT);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, 0);

  Serial.begin(115200);
  Serial.println("#################");
  Serial.println("1 - calibration");
  Serial.println("2 - measurements");
  Serial.println("#################");
}

void processSerial() {
  if(Serial.available() > 0) {
    command = Serial.parseInt();
    switch(command) {
      case 1: {
        Serial.println("Running Calibration...");

        calibration();
        return;
      } break;

      case 2: {
        Serial.println("Running Measurements...");

        // Dummy read otherwise the first measurement is a bit off.
        //analogRead(photoPin);

        running = true;
        current = 0;
        //startTimer();
      } break;
    }
  }
}

unsigned long takeMeasurement() {
  do {
    digitalWrite(ledPin, 1);
    start = micros();
    while(analogRead(photoPin) < threshold);
    end = micros();
    digitalWrite(ledPin, 0);
  } while(start > end); // Repeat the measuremtn if case the timer overflowed

  return end - start;
}

void loop() {
  processSerial();

  while(running) {
    measurements[current++] = takeMeasurement();

    if(current >= measurementCount) {
      running = false;
      printResults();

      return;
    }

    delay(random(randomDelayMin, randomDelayMax));
  }
}
