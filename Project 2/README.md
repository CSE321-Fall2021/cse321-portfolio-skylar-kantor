-------------------
About
-------------------
Project Description: 

A timer, with an LCD screen, LED lights and a keypad, capable of counting down from any time less than or equal to 9 minutes, 59 seconds.
Contribitor List:

Skylar Kantor (alexkant@buffalo.edu)
--------------------
Features
--------------------
-Counts down from any time less than or equal to 9min 59sec
-Runs indefinitely
-User input for time
--------------------
Required Materials
--------------------
-Nucleo L4R5ZI
-1802 LCD
-jumper wires
-breadboard
-resistors
-3 LEDs
--------------------
Resources and References
--------------------
HOW TO MAKE A README.TXT FILE (https://libraries.ou.edu/content/how-make-readmetxt-file)

--------------------
Getting Started
--------------------



--------------------
CSE321_project2_alexkant_main.cpp:
--------------------
This file is where the handling of the keypresses occurs, as well as the timing itself

----------
Variables Declared
----------
InterruptIn oneToSeven: Interrupt object for the first collumn
InterruptIn twoToZero: Interrupt object for the second collumn
InterruptIn threeToNine: Interrupt object for the third collumn
InterruptIn AToD: Interrupt object for the fourth collumn

int row: Used to keep track of which row of the keypad is being powered, since each collumn shares a GPIO pin

CSE321_LCD display: The LCD object

----------
API and Built In Elements Used
----------


InterruptIn: creates a reference to an interrupt with a variable name, and is triggered by a button

  .rise: trigger when the button is depressed

  .fall: trigger when the button is released

wait_us: wait a certain nuber of microseconds

thread_sleep_for: wait a certain number of milliseconds


----------
Custom Functions
----------
void one(void): Handle keypresses in the first collumn of the keypad
void two(void): Handle keypresses in the second collumn of the keypad
void three(void): Handle keypresses in the third collumn of the keypad
void A(void): Handle keypresses in the fourth collumn of the keypad
void LEDFlash(void): Blink an LED connected to PD9 

