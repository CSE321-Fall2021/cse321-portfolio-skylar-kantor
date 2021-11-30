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

-6 LEDs

Resources and References
------

HOW TO MAKE A README.TXT FILE (https://libraries.ou.edu/content/how-make-readmetxt-file) Mbed OS Documentation (https://os.mbed.com/docs/mbed-os/v6.15/)

Getting Started
======

CSE321_project3_alexkant_main.cpp:
------


### Variables Declared



### API and Built In Elements Used

-Ticker

-Watchdog

-Mutex



### Custom Functions

-bool is_dewy(int temp, int hum): Based on the temperature and humidity, determine the dewpoint, and whether or not it has been reached

-int speed (bool is_dewy, int temp, int hum) based on the weather conditions, determine the safest speed to drive
