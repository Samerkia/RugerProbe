//Nick Raffel
//RugerProbe

#include "Arduino.h"
#include <DHT.h>           // Temperature & humidity sensor library

#define LIGHT_SENSOR_LED_IND 2  // Light/Photoresistor indicator LED (ON = working, off = low light level, blink = error) - Red LED
#define DHT_LED_IND 3           // Temp/Humid indicator LED (ON = working, blink/off = error) - Yellow LED
#define DHTPIN 7                // DHT11 data pin
#define DHTTYPE DHT11           // Using DHT11 sensor

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
  int lightLevel = analogRead(PHOTORESISTOR_PIN);

  // Checks light levels and sets LED accordingly
  if (lightLevel < 5) {
    digitalWrite(LIGHT_SENSOR_LED_IND, LOW);
  } else if (lightLevel > 5) { 
    digitalWrite(LIGHT_SENSOR_LED_IND, HIGH);
  } else {
    blinkError("PHOTORESISTOR");
  }

  // Checks DHT11 module and 
  if (DHT_LED_IND) digitalWrite(DHT_LED_IND, HIGH);
  else blinkError("DHT11");

  // Read temperature and humidity
  float temp = dht.readTemperature(); // Read temperature (Celsius)
  float fahr = (temp * 9.0/5.0) + 32.0; // Temperature converted to fahrenheit 
  float humidity = dht.readHumidity(); // Read humidity (%)
  printSensorData(fahr, humidity, lightLevel);
  delay(2000);
}

void printSensorData(float t, float h, int l) {
  // Print sensor values
  Serial.print("Temperature: "); Serial.print(t); Serial.println(" °F");
  Serial.print("Humidity: "); Serial.print(h); Serial.println(" %");
  Serial.print("Light Level: "); Serial.print(l); Serial.println(" lux");
  Serial.println("----------------------------------------");
}

void blinkError(String comp) {
  Serial.println("ERROR WITH PHOTORESISTOR");
  for (int i = 0; i < 3; i++) {
    digitalWrite(LIGHT_SENSOR_LED_IND, HIGH);
    delay(200);
    digitalWrite(LIGHT_SENSOR_LED_IND, LOW);
    delay(200);
  }
}