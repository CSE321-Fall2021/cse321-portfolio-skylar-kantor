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

- Displays speed limit on 7-segment display

- Lowers speed limit for wet road conditions or possible icing

Required Materials
------

- Nucleo L4R5ZI

- 7-segment Display

- jumper wires

- breadboard

- 2 resistors

- 4 LEDs

Resources and References
------

- HOW TO MAKE A README.TXT FILE (https://libraries.ou.edu/content/how-make-readmetxt-file) 

- Mbed OS Documentation (https://os.mbed.com/docs/mbed-os/v6.15/)

- D. Driving, “Driving in wet conditions,” Defensive Driving, 03-Aug-2017. [Online]. Available: https://www.defensivedriving.com/blog/driving-in-wet-road-conditions/.[Accessed: 10-Dec-2021].

- M. G. Lawrence, “The relationship between relative humidity and the dewpoint temperature in moist air: A simple conversion and applications,”Bulletin of the American Meteorological Society, vol. 86, no. 2, pp. 225–234, 2005. 
 
Getting Started
======

CSE321_project3_alexkant_main.cpp:
------


### Variables Declared
- Thread readSensor: Thread used for the EventQueue that the function calls for reading the DHT11 are attached to

- Thread findSpeedLimit: Thread used for the EventQueue that the function calls for calculating the speed limit are attached to

- Thread updateDisplay: Thread used for the EventQueue that the function calls for updating the 7-segment display are attached to

- Thread statusLED: Thread for checking for issues with the DHT11

- EventQueue displayQueue: Queue of funtion calls to update the 7-segment display

- EventQueue speedQueue:  Queue of funtion calls to calculate the speed limit

- EventQueue sensorQueue: Queue of funtion calls to read the temperature and humidity from the DHT11

- DHT11 sensor: temperature and humidity sensor object

- DigitDisplay sign: 7-segment display

- int postedLimit: The speed limit in ideal conditions

- int speedLimit: The calculated speed limit

- int temperature: The temperature from the DHT11

- int humidity: The humidity from the DHT11

- int dewDiff: The difference between the current temperature and dew point, if not zero

- int errorNum: either -1 or -2, if the DHT11 reports an issue

- Mutex tempHumLock: Mutex to protect temperature and humidity values

- Mutex speedLimitLock: Mutex to protect speed limit

- Ticker speedTick: Ticker to call function to calculate the speed limit

- Ticker displayTick: Ticker to call function to update the display

- Ticker sensorTick: Ticker to call function to read the DHT11



### API and Built In Elements Used

- Ticker

- Watchdog

- Mutex

- EventQueue

- Thread



### Custom Functions

- bool is_dewy(): Based on the temperature and humidity, determine the dewpoint, and whether or not it has been reached

- void speed(): based on the weather conditions, determine the safest speed to drive

- void getTempAndHumidity(): read the temperature and humidity from the DHT11

- void displaySpeedLimit(): update the 7-segment display

- void status(): Check if the DHT11 reported any issues during the last read

- void dispatchTemp(): Add a new call to getTempAndHumidity to the queue

- void dispatchDisplay(): add a new call to displaySpeedLimit to the queue

- void dispatchSpeed(): add a new call to speed to the display
