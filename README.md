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
  1. Rocket
      - 2-Stage ground-;it booster with TVC 
      - 74cm length, 7.62 diameter, 662g dry mass
      - F-15 Motors 

  2. Flight Computer 
      - STM32F722RET6 MicroController
      - ICM-45686 IMU 
      - BMP580 Barometer
      - BQ25883 Battery Charger 
      - 4 channel servo PWM 
      - microSD Card for logging

  3. Launchpad 
      - ESP32 MicroController
      - 2 control arms that hold and release the rocket at liftoff 
      - 2 ESP32-CAM modules that record the launch 
      - RGB strip light and 4 2812 LEDs for pad status/lighting 
      - Water deluge system for pad deck 
      
  4. Firmware
  The flight computer is written from scratch organized into driver-level code (Core/) and mission appliccation logic (App/), covering the state machine, sensor fusion (Kalman filtering), pyro firing logic, servo/TVC Control, GPS parsing, DFU handoff, and the onboard     parameter store. Safety critical paths, particularly pyro firing, use redundant latching to prevent unintended actuation.  

# CAD:[onshape Link]([url](https://cad.onshape.com/documents/130dc36974a4df304d494706/w/43298f754d2d228858bc474d/e/d549df94310ea82951faf52f?renderMode=0&uiState=6a8dd186323a86b6993efd52))

 |Category                              |Name                              |Price  |Source                                                                      |Purpose                                  |FIELD6|Quantity|
|--------------------------------------|----------------------------------|-------|----------------------------------------------------------------------------|-----------------------------------------|------|--------|
|Electronics                           |Flight Computer                   |$220.00|JLCPCB                                                                      |Main Flight Computer                     |      |1       |
|                                      |ESP32 type-c FT232                |$4.60  |https://tinyurl.com/yhpx8n6v                                                |MicroController for Launchpad            |      |1       |
|                                      |ESP32 Expansion Board             |$2.60  |https://tinyurl.com/23y7nzj4                                                |Expansion Board                          |      |1       |
|                                      |Esp32 Cam                         |$6.80  |https://tinyurl.com/4du8rxrx                                                |Camera Board for recording               |      |1       |
|                                      |WS2812 Breakout                   |$1.10  |https://tinyurl.com/47v776rm                                                |Lights for Launchpad                     |      |4       |
|                                      |RGB Strip Light                   |$5.10  |https://tinyurl.com/364y65ux                                                |Lights for Launchpad                     |      |1       |
|                                      |SG90 Servos                       |$9.00  |https://tinyurl.com/mr2s3xp6                                                |Servos for TVC and Launchpad Control Arms|      |5       |
|                                      |DRV8871 H-Bridge Motor Driver     |$2.65  |https://tinyurl.com/32cf8wx4                                                |Motor Driver for Water Deluge System     |      |1       |
|                                      |Jumper Wires                      |$4.70  |https://tinyurl.com/58u2vc35                                                |Wires                                    |      |1       |
|                                      |22 AWG Wire                       |$3.39  |https://tinyurl.com/bdz5rad7                                                |Wires                                    |      |1       |
|                                      |2.54mm Screw Terminal             |$5.22  |https://tinyurl.com/4t54j69v                                                |Screw Terminal                           |      |2       |
|                                      |JST XH2.54 Pitch 2 Pin Male Female|$2.29  |https://tinyurl.com/397bn54r                                                |Battery Connector                        |      |1       |
|                                      |Micro SD Card                     |$7.70  |https://tinyurl.com/yx3v2jh6                                                |SD Card for Data logging                 |      |1       |
|                                      |OV5640 Camera                     |$5.47  |https://tinyurl.com/mv5eb5tb                                                |Camera For recording                     |      |1       |
|                                      |DC Sumersible Pump                |$2.43  |https://tinyurl.com/bp4uh89j                                                |Water Pump                               |      |1       |
|                                      |2S Lipo                           |$4.60  |https://tinyurl.com/3x3pstst                                                |Battery                                  |      |1       |
|Category                              |Name                              |Price  |Source                                                                      |Purpose                                  |      |Quantity|
|Hardware                              |3.00" Airframe Tubing             |$14.16 |https://locprecision.com/products/cardboard-airframes?variant=39778656125119|Main Body Tube                           |      |1       |
|                                      |Hot Glue Sticks                   |$2.17  |https://tinyurl.com/yvwvy5t6                                                |Adhesive                                 |      |1       |
|                                      |Epoxy                             |$3.42  |https://tinyurl.com/9rpxazkp                                                |Adhesive                                 |      |1       |
|                                      |Silicone Tube                     |$2.80  |https://tinyurl.com/3bzxhtnu                                                |Tubing for Water Deluge System           |      |1       |
|                                      |Header Pins                       |$3.63  |https://tinyurl.com/u3jnrvzz                                                |Header Pins                              |      |4       |
|                                      |                                  |       |                                                                            |                                         |      |        |
|Below Totals contain Tax and Shipping |                                  |       |                                                                            |                                         |      |        |
|                                      |                                  |       |                                                                            |                                         |      |        |
|JLCPCB                                |$350.00                           |       |                                                                            |                                         |      |        |
|Aliexpress Total                      |$111.09                           |       |                                                                            |                                         |      |        |
|LOC Total                             |$28.30                            |       |                                                                            |                                         |      |        |
|                                      |                                  |       |                                                                            |                                         |      |        |
|                                      |                                  |       |                                                                            |                                         |      |        |
|Grant Total:                          |$489.39                           |       |                                                                            |                                         |      |        |

