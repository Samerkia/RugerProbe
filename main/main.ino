//Nick Raffel
//RugerProbe

#include "Arduino.h"
#include <DHT.h>           // Temperature & humidity sensor library
#include <ArduinoBLE.h>
#include <WiFiNINA.h>
#include "secrets.h"

// GPIO 1-6 will be LEDS for now
#define LIGHT_SENSOR_LED_IND 2  // Light/Photoresistor indicator LED (ON = working, off = low light level, blink = error) - Red LED
#define DHT_LED_IND 3           // Temp/Humid indicator LED (ON = working, blink/off = error) - Yellow LED
#define ULTRA_SONIC_LED_IND 4           // Distance indicator LED (ON = working, blink/off = error) - BLUE LED
#define MOTION_LED_IND 5           // Motion indicator LED (ON = working, blink/off = error) - GREEN LED

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

char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;
int status = WL_IDLE_STATUS;
WiFiServer server(6969);
// BLEService dataService("180A");
// BLEStringCharacteristic stringCharacteristic("2A57", BLERead | BLEWrite, 256);

// Initialize sensors
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  while(!Serial);
  
  pinMode(PHOTORESISTOR_PIN, INPUT); // Analog
  pinMode(LIGHT_SENSOR_LED_IND, OUTPUT); // indicator output
  pinMode(DHT_LED_IND, OUTPUT); // indicator output
  pinMode(ULTRA_SONIC_LED_IND, OUTPUT);
  pinMode(MOTION_LED_IND, OUTPUT);

  pinMode(SONIC_PIN, OUTPUT); // Set SONIC_PIN as OUTPUT for triggering ultrasonic sensor
  pinMode(ECHO_PIN, INPUT); // Set ECHO_PIN as INPUT for receiving echo signal

  pinMode(MOTION_SENSOR_PIN, INPUT); // Motion Sensor PIN
  
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to network: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // if (!BLE.begin()) {
  //   Serial.println("b Error With Bluetooth");

  //   while(1);
  // }

  // BLE.setLocalName("RugerProbe");
  // BLE.setAdvertisedService(dataService);
  // dataService.addCharacteristic(stringCharacteristic);
  // BLE.addService(dataService);
  // BLE.advertise();
  // Serial.println("BLE Peripheral Started - waiting for connection");

  dht.begin();
  server.begin();
  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");
  Serial.println("---------------------------------------");

}
int i = 0;
void loop() {
  // BLEDevice central = BLE.central();  
  WiFiClient client = server.available();
  // if (central) {
  if (client) {
    Serial.println("New client connected BRUH");
    //while (central.connected()) {
    while (client.connected()) {
      // Serial.println(central.address());
      // put your main code here, to run repeatedly:
      int lightLevel = analogRead(PHOTORESISTOR_PIN);//getLightLevels();  
      validateLightSensor(lightLevel);
      float temp, humidity;
      getTempData(temp, humidity);
      float dist = getSonar();
      bool isMotion = monitorMotion();
      // if (central.connected()) {
      //   sendDataViaBluetooth(temp, humidity, lightLevel, dist, isMotion);
      // }
      
      client.print("HTTP/1.1 200 OK\r\n");
      client.print("Content-Type: text/plain\r\n");
      client.print("Connection: close\r\n\r\n");

      // client.print(data + String(i));
      printSensorData(temp, humidity, lightLevel, dist, isMotion, client);

      delay(1000);
      // delay(1);
      client.stop();
      Serial.println("Client disconnected");
      // }
      // } else {
      //   Serial.println("FAILURE PEICE OF SHIT");
      // }
    }
  }
}

bool monitorMotion() {
  digitalWrite(MOTION_LED_IND, HIGH);
  if (digitalRead(MOTION_SENSOR_PIN) == HIGH || digitalRead(MOTION_SENSOR_PIN) == LOW) {  
    // If sensor responds with either HIGH or LOW, it's working.
    return digitalRead(MOTION_SENSOR_PIN) == HIGH;
  } 
  blinkError("MOTION");
  return false;
}

float getSonar() {
  long pingTime;
  float dist;
  
  // Ensure the trigger pin is LOW before sending the pulse
  digitalWrite(SONIC_PIN, LOW); 
  delay(1);
  
  // Trigger the ultrasonic sensor
  digitalWrite(SONIC_PIN, HIGH);
  delay(10);  // Send a 10-microsecond pulse
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
  
  // Checks DHT11 module is working 
  if (dist > 0 && dist < 400) digitalWrite(ULTRA_SONIC_LED_IND, HIGH);
  else blinkError("DHT11");

  return dist;
}


void getTempData(float &temp, float &humidity) {
  temp = dht.readTemperature(); // Read temperature (Celsius)
  // Checks DHT11 module is working 
  if (!isnan(temp)) digitalWrite(DHT_LED_IND, HIGH);
  else blinkError("DHT11");
  // Read temperature and humidity
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

void printSensorData(float t, float h, int l, float d, bool m, WiFiClient c) {
  // Print sensor values
  Serial.print("t "); Serial.print(t); Serial.println(" F");
  c.print("t "); c.print(t); c.println(" F");
  Serial.print("h "); Serial.print(h); Serial.println(" %");
  c.print("h "); c.print(h); c.println(" %");
  Serial.print("l "); Serial.print(l); Serial.println(" lux");
  c.print("l "); c.print(l); c.println(" lux");
  Serial.print("d "); Serial.print(d); Serial.println(" cm away from object");
  c.print("d "); c.print(d); c.println(" cm away from object");
  if (m) {
    Serial.print("m "); Serial.println("MOTION DETECTED");
    c.print("m "); c.println("MOTION DETECTED");
  } else {
    Serial.print("m "); Serial.println("Monitoring For Motion");
    c.print("m "); c.println("Monitoring For Motion");
  }
}

void sendDataViaBluetooth(float t, float h, int l, float d, bool m) {
  String data = String("t: ") + t + 
                String(",h: ") + h + 
                String(",l: ") + l + 
                String(",d: ") + d + 
                String(",m: ") + (m ? "MOTION DETECTED" : "Monitoring For Motion");

  // stringCharacteristic.writeValue(data);
}

void blinkError(String comp) {
  // Serial.println("ERROR " + comp);
  int pin;
  if (comp == "PHOTORESISTOR") pin = LIGHT_SENSOR_LED_IND;
  else if (comp == "DHT11") pin = DHT_LED_IND;
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
