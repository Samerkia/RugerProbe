//Nick Raffel
//RugerProbe

#include "Arduino.h"
#include <DHT.h>           // Temperature & humidity sensor library

// GPIO 1-6 will be LEDS for now
#define LIGHT_SENSOR_LED_IND 2  // Light/Photoresistor indicator LED (ON = working, off = low light level, blink = error) - Red LED
#define DHT_LED_IND 3           // Temp/Humid indicator LED (ON = working, blink/off = error) - Yellow LED
#define ULTRA_SONIC_LED_IND 4           // Temp/Humid indicator LED (ON = working, blink/off = error) - BLUE LED
#define MOTION_LED_IND 5           // Temp/Humid indicator LED (ON = working, blink/off = error) - GREEN LED

// GPIO 7 - 13 are devices 
#define DHTPIN 7                // DHT11 data pin
#define DHTTYPE DHT11           // Using DHT11 sensor

#define SONIC_PIN 9             // UltraSonic Modulator Pin
#define ECHO_PIN 10              // UltraSonic Modulator Echoing Pin
#define MAX_DISTANCE 220        // max distance to measure
#define timeOut MAX_DISTANCE*60 // Timeout according to the max distance measured

#define MOTION_SENSOR_PIN 8

// Analog Photoresistor pin
const byte PHOTORESISTOR_PIN = A0; // Light reader pin

// Initialize sensors
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // put your setup code here, to run once:
  pinMode(PHOTORESISTOR_PIN, INPUT); // Analog
  pinMode(LIGHT_SENSOR_LED_IND, OUTPUT); // indicator output
  pinMode(DHT_LED_IND, OUTPUT); // indicator output
  pinMode(ULTRA_SONIC_LED_IND, OUTPUT);
  pinMode(MOTION_LED_IND, OUTPUT);

  pinMode(SONIC_PIN, OUTPUT); // Set SONIC_PIN as OUTPUT for triggering ultrasonic sensor
  pinMode(ECHO_PIN, INPUT); // Set ECHO_PIN as INPUT for receiving echo signal

  pinMode(MOTION_SENSOR_PIN, INPUT); // Motion Sensor PIN
  
  dht.begin();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int lightLevel = analogRead(PHOTORESISTOR_PIN);//getLightLevels();  
  validateLightSensor(lightLevel);
  float temp, humidity;
  getTempData(temp, humidity);
  float dist = getSonar();
  bool isMotion = monitorMotion();
  printSensorData(temp, humidity, lightLevel, dist, isMotion);
  delay(1);
}

bool monitorMotion() {
  if (MOTION_LED_IND) digitalWrite(MOTION_LED_IND, HIGH);
  else blinkError("MOTION");
  if (digitalRead(MOTION_SENSOR_PIN) == HIGH) {
    return true;
  }
  return false;
}

unsigned long pulseIn(int pin, int level, int timeout) {
  unsigned long startTime = micros();

   // Wait for the pulse to START
  while (digitalRead(pin) != level) {
    if (micros() - startTime > timeout) {
      // Serial.println("TIMEOUT waiting for pulse start");
     blinkError("SONAR");
      return 0; // Timeout
    }
  }

  unsigned long pulseStart = micros();

  // Wait for the pulse to END
  while (digitalRead(pin) == level) {
    if (micros() - startTime > timeout) {
      // Serial.println("TIMEOUT waiting for pulse end");
     blinkError("SONAR");
      return 0; // Timeout
    }
  }

  // Return the duration of the pulse in microseconds
  return micros() - pulseStart;
}

float getSonar() {
  
  // Checks DHT11 module is working 
  if (ULTRA_SONIC_LED_IND) digitalWrite(ULTRA_SONIC_LED_IND, HIGH);
  else blinkError("DHT11");

  long pingTime;
  float dist;
  
  // Ensure the trigger pin is LOW before sending the pulse
  digitalWrite(SONIC_PIN, LOW); 
  delayMicroseconds(2);
  
  // Trigger the ultrasonic sensor
  digitalWrite(SONIC_PIN, HIGH);
  delayMicroseconds(10);  // Send a 10-microsecond pulse
  digitalWrite(SONIC_PIN, LOW);
  
  // Measure the time it takes for the echo to return
  pingTime = pulseIn(ECHO_PIN, HIGH, timeOut);
  
  // If no valid pulse time is returned, print a warning
  if (pingTime == 0) {
    // Serial.println("No pulse received from ultrasonic sensor.");
    // Checks light levels and sets LED accordingly
   blinkError("SONAR");
    return 0;  // Return 0 to indicate failure
  }

  // Calculate the distance based on the speed of sound
  dist = (float)pingTime * 340.0 / 2.0 / 10000.0;  // Distance in cm
  
  return dist;
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

void validateLightSensor(int l) {
    // Checks light levels and sets LED accordingly
    if (l < 5) {
      digitalWrite(LIGHT_SENSOR_LED_IND, LOW);
    } else if (l > 5) { 
      digitalWrite(LIGHT_SENSOR_LED_IND, HIGH);
    } else {
      blinkError("PHOTORESISTOR");
    }
}

void printSensorData(float t, float h, int l, float d, bool m) {
  // Print sensor values
  Serial.print("t "); Serial.print(t); Serial.println(" F");
  Serial.print("h "); Serial.print(h); Serial.println(" %");
  Serial.print("l "); Serial.print(l); Serial.println(" lux");
  Serial.print("d "); Serial.print(d); Serial.println(" cm away from object");
  if (m) {
    Serial.print("m "); Serial.println("MOTION DETECTED");
  } else {
    Serial.print("m "); Serial.println("Monitoring For Motion");
  }
}

void blinkError(String comp) {
  // Serial.println("ERROR " + comp);
  int pin;
  if (comp == "PHOTORESISTOR") pin = LIGHT_SENSOR_LED_IND;
  else if (comp == "DHT11") pin = DHTPIN;
  else if (comp == "SONAR") pin = ULTRA_SONIC_LED_IND;
  else if (comp == "MOTION") pin = ULTRA_SONIC_LED_IND;
  else pin = 0;
  for (int i = 0; i < 3; i++) {
    digitalWrite(pin, HIGH);
    delay(200);
    digitalWrite(pin, LOW);
    delay(200);
  }
}