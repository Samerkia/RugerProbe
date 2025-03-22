# RugerProbe
This is a probe device named after my dog, Ruger. It will read telemetry data and transmit it to a front end app with eventual wireless communication!

Current component list
- Arduino Uno (in the prototype phase, if built may use Arduino Nano 33)
- temperature and humidity levels (DHT11) 
- light levels (photoresistor [Might switch to TSL2562])
- PIR Motion Sensor
- Measure distance with Ultrasonic Ranging Module (HC-SR04)

## Future updates
Hardware
- Implement a way to take pictures
- Switch to Arduino Nano 33 then I can transmit data wirelessly
- Design a custom PCB to use instead of a bread board
- Implement a way to add in motors and wheels
- If I add in motors and wheels, I will need to add in a way to read acceleration and a gyroscope (Attitude Sensor MPU6050)
  
Code
- Code to make the Hardware goals work
- Make the data collection app look awesome
- Maybe a way to send data from front end to the arduino

# Running the app
- Have the Arduino run the main.ino code
- Run the backend code `dotnet run .\serialDataHandler.cs`
- Run the frontend code `npx electron .`
- Eventually I will make it into one executable

# Current Status <3/22/25>
## Code updates
I can now send and receive data via wifi! I did wifi instead of Bluetooth cuz I couldn't get BT to work and wifi has bigger range anyways.
## Data Collection Screens
3/2/25 - Displays all data collected (Temperature, Humidity, Light Level, Distance an object is, and if Motion is detected or not), and it is now stylized! (I will keep updating as I see fit as well)

![data4](https://github.com/user-attachments/assets/6c0bf008-d639-4f61-9d56-4cae582d360c)


## Hardware
3/16/25 - Ardunio Nano and wireless power added.... working on a way to read BLE data but struggling to get code to connect... my phone can read it though 

![proto3](https://github.com/user-attachments/assets/b8c98332-8be4-4791-850e-21aa27ceb886)

