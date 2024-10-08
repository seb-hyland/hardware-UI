#include <DHT.h>
#include <Keypad.h>

const int DHTPIN = 13;
DHT dht(DHTPIN, DHT11);

const int MOTOR_PIN_1 = 3;
const int MOTOR_PIN_2 = 4;
const int SPEED_PIN = 2;

bool mixingLiquid = true; // whether pushing air or liquid during mixing
unsigned long curTime = 0;
unsigned long lastMix = 0;
int progIndex = 1;

char hexaKeys[2][2] = {
  {'1', '2'},
  {'4', '5'}
};

byte rowPins[2] = {8, 7};
byte colPins[2] = {6, 5};
Keypad controlPad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, 2, 2);

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(SPEED_PIN, OUTPUT);
} 

void dhtRead() {
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  if (isnan(temp) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
  }
  else {
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print("°C\tHumidity: ");
    Serial.print(humidity);
    Serial.println("%");
  }
}

void fluidIn(int flow) {
  analogWrite(SPEED_PIN, flow);
  digitalWrite(MOTOR_PIN_1, HIGH);
  digitalWrite(MOTOR_PIN_2, LOW);
}

void fluidOut(int flow) {
  analogWrite(SPEED_PIN, flow);
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, HIGH);
}

void mixer(int flow, int time) {
  if (mixingLiquid == true && curTime > (lastMix + time)) {
    fluidIn(flow);
    lastMix = curTime;
    mixingLiquid = false;
  } 
  else if (mixingLiquid == false && curTime > (lastMix + (time * 2))) {
    fluidOut(flow);
    lastMix = curTime;
    mixingLiquid = true;
  } 
}

void readOD(int flow) {
  fluidIn(flow);
  delay(2250);
  fluidOut(flow);
  delay(3500);
}

void off() {
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, LOW);
}

void runProg(int flow, int time) {
  switch (progIndex) {
  case 1:
    off();
    break;
  case 2:
    mixer(flow, time);
    break;
  case 4:
    fluidIn(flow);
    break;
  case 5:
    fluidOut(flow);
    break;
  }
}

void readNumpad() {
  char readingKey = controlPad.getKey();
  progIndex = (readingKey) ? readingKey - '0' : progIndex;
  if (readingKey) {
    Serial.print("Key: ");
    Serial.print(readingKey);
    Serial.print("/tProg: ");
    Serial.println(progIndex);
  }
}

void loop() {
  curTime = millis();
  //  dhtRead();
  readNumpad();
  runProg(220, 7000);
// fluidIn(200);
}
