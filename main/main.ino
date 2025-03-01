//Nick Raffel
//RugerProbe

#include "Arduino.h"
#include <DHT.h>           // Temperature & humidity sensor library

#define LIGHT_SENSOR_LED_IND 2  // Light/Photoresistor indicator LED (ON = working, off = low light level, blink = error) - Red LED
#define DHT_LED_IND 3           // Temp/Humid indicator LED (ON = working, blink/off = error) - Yellow LED
#define DHTPIN 7                // DHT11 data pin
#define DHTTYPE DHT11           // Using DHT11 sensor
#define SONIC_PIN 4             // UltraSonic Modulator Pin
#define ECHO_PIN 5              // UltraSonic Modulator Echoing Pin
#define MAX_DISTANCE 220        // max distance to measure
#define timeOut MAX_DISTANCE*60 // Timeout according to the max distance measured

const byte PHOTORESISTOR_PIN = A0; // Light reader pin

// Initialize sensors
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // put your setup code here, to run once:
  pinMode(PHOTORESISTOR_PIN, INPUT); // Analog

  pinMode(LIGHT_SENSOR_LED_IND, OUTPUT); // indicator output
  pinMode(DHT_LED_IND, OUTPUT); // indicator output
  
  dht.begin();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int lightLevel = getLightLevels();  
  float temp, humidity;
  getTempData(temp, humidity);
  float dist = getSonar();
  printSensorData(temp, humidity, lightLevel, dist);
  delay(1);
}


unsigned long pulseIn(int pin, int level, int timeout) {
  unsigned long startTime = micros();

  //wait for the pulse to start
  while(digitalRead(pin) != level) {
    if (micros() - startTime > timeout) return 0; // timeout
  }

  unsigned long pulseStart = micros();

  //wait for the pulse to start
  while(digitalRead(pin) != level) {
    if (micros() - startTime > timeout) return 0; // timeout
  }

  return micros() - pulseStart;
}

// gets the measurement result of the ultra sonic modulator in centimeters
float getSonar() {
  long pingTime;
  float dist;
  digitalWrite(SONIC_PIN, HIGH); 
  delayMicroseconds(10);
  digitalWrite(SONIC_PIN, LOW);
  pingTime = pulseIn(ECHO_PIN, HIGH, timeOut); // read the puls time of the echo pin
  dist = (float)pingTime * 340.0 / 2.0 / 10000.0; // Calculate distance using speed of sound
}

void getTempData(float &temp, float &humidity) {
  // Checks DHT11 module is working 
  if (DHT_LED_IND) digitalWrite(DHT_LED_IND, HIGH);
  else blinkError("DHT11");

  // Read temperature and humidity
  temp = dht.readTemperature(); // Read temperature (Celsius)
  temp = (temp * 9.0/5.0) + 32.0; // Temperature converted to fahrenheit 
  humidity = dht.readHumidity(); // Read humidity (%)
}

int getLightLevels() {
    int lightLevel = analogRead(PHOTORESISTOR_PIN);

    // Checks light levels and sets LED accordingly
    if (lightLevel < 5) {
      digitalWrite(LIGHT_SENSOR_LED_IND, LOW);
    } else if (lightLevel > 5) { 
      digitalWrite(LIGHT_SENSOR_LED_IND, HIGH);
    } else {
      blinkError("PHOTORESISTOR");
    }

    return lightLevel;
}

void printSensorData(float t, float h, int l, float d) {
  // Print sensor values
  Serial.print("t "); Serial.print(t); Serial.println(" F");
  Serial.print("h "); Serial.print(h); Serial.println(" %");
  Serial.print("l "); Serial.print(l); Serial.println(" lux");
  Serial.print("d "); Serial.print(d); Serial.println(" cm away from object");
}

void blinkError(String comp) {
  Serial.println("ERROR " + comp);
  int pin;
  if (comp == "PHOTORESISTOR") pin = LIGHT_SENSOR_LED_IND;
  else if (comp == "DHT11") pin = DHTPIN;
  for (int i = 0; i < 3; i++) {
    digitalWrite(pin, HIGH);
    delay(200);
    digitalWrite(pin, LOW);
    delay(200);
  }
}