
const int soundSensor = 2;
const int ledPin = 8;

const float MIN_FREQ = 100.0;
const float MAX_FREQ = 10000.0;

volatile unsigned long pulseCount = 0;

bool ledState = false;
bool waitingForQuiet = false;

unsigned long lastValidSound = 0;
unsigned long lastWindow = 0;

const unsigned long sampleWindow = 20;
const unsigned long quietTime = 250;

void countPulse() {
  pulseCount++;
}

void setup() {
  pinMode(soundSensor, INPUT);
  pinMode(ledPin, OUTPUT);

  digitalWrite(ledPin, LOW);
  Serial.begin(9600);

  attachInterrupt(
    digitalPinToInterrupt(soundSensor),
    countPulse,
    RISING
  );

  lastWindow = millis();
}

void loop() {
  unsigned long now = millis();

  if (now - lastWindow >= sampleWindow) {
    noInterrupts();
    unsigned long pulses = pulseCount;
    pulseCount = 0;
    interrupts();

    float frequency =
      pulses * 1000.0 / sampleWindow;

    Serial.print("Estimated frequency: ");
    Serial.print(frequency);
    Serial.println(" Hz");

    bool validFrequency =
      frequency >= MIN_FREQ &&
      frequency <= MAX_FREQ;

    if (validFrequency) {
      lastValidSound = now;

      if (!waitingForQuiet) {
        ledState = !ledState;
        digitalWrite(ledPin, ledState ? HIGH : LOW);

        Serial.println(ledState ? "LED ON" : "LED OFF");

        waitingForQuiet = true;
      }
    }

    if (waitingForQuiet &&
        now - lastValidSound >= quietTime) {
      waitingForQuiet = false;
    }

    lastWindow = now;
  }
}
