// Setup pins to your liking
const int photoPin = A0;
const int ledPin = 13;

const unsigned int measurementCount = 100;
unsigned long measurements[measurementCount];

unsigned int current;
unsigned int command;

unsigned long threshold = 10;

unsigned long start;
unsigned long end;
unsigned int i;

const unsigned int randomDelayMin = 500;
const unsigned int randomDelayMax = 1000;

bool calibrated = false;
bool running = false;
const unsigned long measurementTimeoutUs = 5000000; // 5 seconds

void calibration() {
  digitalWrite(ledPin, 0);
  delay(500);
  unsigned long calMin = analogRead(photoPin);

  digitalWrite(ledPin, 1);
  delay(500);
  unsigned long calMax = analogRead(photoPin);
  digitalWrite(ledPin, 0);

  /* Threshold is set to 1/4 of the calibrated range above the minimum.
   *
   * A lower threshold detects faster but is more sensitive to noise.
   * A higher threshold is more reliable but adds latency from the LED
   * needing to get bright enough. 1/4 of the range is a good compromise.
   */
  threshold = calMin + (calMax - calMin) / 4;

  Serial.print("Min: ");
  Serial.print(calMin);
  Serial.print(" Max: ");
  Serial.print(calMax);
  Serial.print(" Threshold: ");
  Serial.println(threshold);

  calibrated = true;
}

void printResults() {
  unsigned long resultMin = measurements[0];
  unsigned long resultMax = 0;
  unsigned long resultAvg = 0;

  Serial.println("##### START #####");
  for(i = 0; i < measurementCount; i += 1) {
    if(measurements[i] > resultMax) {
      resultMax = measurements[i];
    }

    if(measurements[i] < resultMin) {
      resultMin = measurements[i];
    }

    resultAvg += measurements[i];
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
  Serial.print("Min:\t");
  Serial.print(resultMin);
  Serial.print("us\t(");
  Serial.print(resultMin / 1000);
  Serial.println("ms)");
  Serial.print("Max:\t");
  Serial.print(resultMax);
  Serial.print("us\t(");
  Serial.print(resultMax / 1000);
  Serial.println("ms)");
  Serial.print("Avg:\t");
  Serial.print(resultAvg / measurementCount);
  Serial.print("us\t(");
  Serial.print(resultAvg / measurementCount / 1000);
  Serial.println("ms)");
  
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
        if(!calibrated) {
          Serial.println("Please run calibration first!");
          return;
        }

        Serial.println("Running Measurements...");

        running = true;
        current = 0;
      } break;
    }
  }
}

// Returns 0 on timeout
unsigned long takeMeasurement() {
  do {
    digitalWrite(ledPin, 1);
    start = micros();
    while(analogRead(photoPin) < threshold) {
      if(micros() - start > measurementTimeoutUs) {
        digitalWrite(ledPin, 0);
        return 0;
      }
    }
    end = micros();
    digitalWrite(ledPin, 0);
  } while(start > end); // Repeat the measurement in case the timer overflowed

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
