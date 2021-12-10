About
======

Project Description:
------

A speed limit sign, that changes the speed limit displayed based on the temperature and humidity

Contribitor List:
------

Skylar Kantor (alexkant@buffalo.edu)

Features
------

-Displays speed limit on 7-segment display

-Lowers speed limit for wet road conditions or possible icing

Required Materials
------

-Nucleo L4R5ZI

-7-segment Display

-jumper wires

-breadboard

-resistors

-4 LEDs

Resources and References
------

HOW TO MAKE A README.TXT FILE (https://libraries.ou.edu/content/how-make-readmetxt-file) 

Mbed OS Documentation (https://os.mbed.com/docs/mbed-os/v6.15/)

D. Driving, “Driving in wet conditions,” Defensive Driving, 03-Aug-2017. [Online]. Available: https://www.defensivedriving.com/blog/driving-in-wet-road-conditions/.[Accessed: 10-Dec-2021].

 M. G. Lawrence, “The relationship between relative humidity and the dewpoint temperature in moist air: A simple conversion and applications,”Bulletin of the American Meteorological Society, vol. 86, no. 2, pp. 225–234, 2005. 
 
Getting Started
======

CSE321_project3_alexkant_main.cpp:
------


### Variables Declared
Thread readSensor

Thread findSpeedLimit

Thread updateDisplay

Thread statusLED

EventQueue displayQueue

EventQueue speedQueue

EventQueue sensorQueue

DHT11 sensor(PA_5)

DigitDisplay sign(PA_6, PA_7)

int postedLimit

int speedLimit

int temperature

int humidity

int dewDiff

int errorNum

Mutex tempHumLock

Mutex speedLimitLock

Ticker speedTick

Ticker displayTick

Ticker sensorTick



### API and Built In Elements Used

-Ticker

-Watchdog

-Mutex

-EventQueue

-Thread



### Custom Functions

-bool is_dewy(): Based on the temperature and humidity, determine the dewpoint, and whether or not it has been reached

-void speed () based on the weather conditions, determine the safest speed to drive

-void getTempAndHumidity()

-void displaySpeedLimit()

-void status()

-void dispatchTemp()

-void dispatchDisplay()

-void dispatchSpeed()
