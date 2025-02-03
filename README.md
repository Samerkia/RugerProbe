# RugerProbe
This is a probe device named after my dog Ruger. It will read telemetry data and store it with eventual wireless communication

## WIP (This is in the early stages as of 1/31/25) 
I am currently designing and picking the hardware to use/designing the code behind it. I will add pictures to the read-me when available

- Arduino Uno (in the prototype phase, if built may use Arduino Nano)
- temperature and humidity levels (DHT11) 
- light levels (photoresistor [Might switch to TSL2562])
- PIR Motion Sensor
- Measure distance with Ultrasonic Ranging Module (HC-SR04)


## RugerProbe Prototype (2/2/2025)
Here is an image of where it is currently at. It reads Light levels, temperature, and humidity. There are LED indicator lights that show if everything is working. The picture makes it hard to see but the RED LED = Photoresistor and the YELLOW LED = DHT11 module. I plan to add more per each module I add to this probe

![ProtoType1](https://github.com/user-attachments/assets/9ea3d890-8a68-4ae9-9713-37ff85346912)

Here is a screenshot of the data it collects at the moment

![Data1](https://github.com/user-attachments/assets/70875138-2e4b-4b8a-8d1e-ed0a5d3d8d71)
