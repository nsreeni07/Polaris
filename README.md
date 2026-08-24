# Polaris
Polaris is a two-stage amateur rocket with active thrust vector control(TVC) on the booster, along with a custom flight computer, ground support hardware, and firmware. 

# Why
Most amateur rocketry projects rely on passive stablization:fins,high-TWR motors. Polaris was build to go further. It uses a ground lit booster stage with activle gimbal control instead of fins. Then after apogee the second motor lights hot-staging the booster stage off. Then paired with the flight computer the rocket stablizes and lands by itself

# Breakdown
Polaris is made up of 3 main subsystems
 1.Rocket-Airframe,Motors
 2.Flight Computer-Custom electronics and firmware that control the veichles orientation 
 3.Launchpad - the ground support equipment that holds, arms, and releases the rocket 

# Subsystems In Depth 
  1.Rocket
      * 2-Stage ground-;it booster with TVC 
      * 74cm length, 7.62 diameter, 662g dry mass
      * F-15 Motors 

  2.Flight Computer 
      * STM32F722RET6 MicroController
      * ICM-45686 IMU 
      * BMP580 Barometer
      * BQ25883 Battery Charger 
      * 4 channel servo PWM 
      * microSD Card for logging

  3.Launchpad 
      * ESP32 MicroController
      * 2 control arms that hold and release the rocket at liftoff 
      * 2 ESP32-CAM modules that record the launch 
      * RGB strip light and 4 2812 LEDs for pad status/lighting 
      * Water deluge system for pad deck 
      
  4.Firmware
  The flight computer is written from scratch organized into driver-level code (Core/) and mission appliccation logic (App/), covering the state machine, sensor fusion (Kalman filtering), pyro firing logic, servo/TVC Control, GPS parsing, DFU handoff, and the onboard     parameter store. Safety critical paths, particularly pyro firing, use redundant latching to prevent unintended actuation.  
 
      
