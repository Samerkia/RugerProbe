# RugerProbe
This is a probe device named after my dog Ruger. It will read telemetry data and store it with eventual wireless communication

## WIP (This is in the early stages <del>as of 1/31/25</del>) 
I am currently designing and picking the hardware to use/designing the code behind it. I will add pictures to the read-me when available

Current component list
- Arduino Uno (in the prototype phase, if built may use Arduino Nano)
- temperature and humidity levels (DHT11) 
- light levels (photoresistor [Might switch to TSL2562])
- PIR Motion Sensor
- Measure distance with Ultrasonic Ranging Module (HC-SR04)

Future updates
- Implement the PIR Motion Sensor and Ultrasonic Ranging Module
- Implement a way to take pictures
- Implement a way to add in motors and wheels
- Switch to Arduino Nano 33 then I can transmit data wirelessly
- Design a custom PCB to use instead of a bread board
- Add wheels/motors so it's mobile


## RugerProbe Prototype <del>(2/2/2025)</del> (2/27/25)
2/27/25 - I have created a way to collect the data read over the serial port connection (as I do not have a wifi capable Arduino yet) and send data to a front end to display the data better. I plan on making it look cooler at a later date for now just trying to get base functionality set up.

![data2](https://github.com/user-attachments/assets/5b4819fd-e014-47bc-a6ec-f23cdb87e2ce)

2/2/25 - Here is an image of where the physical component is currently at. It reads Light levels, temperature, and humidity. There are LED indicator lights that show if everything is working. The picture makes it hard to see but the RED LED = Photoresistor and the YELLOW LED = DHT11 module. I plan to add more per each module I add to this probe

![ProtoType1](https://github.com/user-attachments/assets/9ea3d890-8a68-4ae9-9713-37ff85346912)

Here is a screenshot of the data it collects at the moment

![Data1](https://github.com/user-attachments/assets/70875138-2e4b-4b8a-8d1e-ed0a5d3d8d71)
