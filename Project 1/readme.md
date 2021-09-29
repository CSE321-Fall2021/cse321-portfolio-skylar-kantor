-------------------
About
-------------------
Project Description: 

Introduction to coding, documentation, and planning best practices, and setup of gitHub and Mbed Studio

Contribitor List:

Skylar Kantor (alexkant@buffalo.edu)

--------------------
Required Materials
--------------------
-Nucleo L4R5ZI

--------------------
Resources and References
--------------------
HOW TO MAKE A README.TXT FILE (https://libraries.ou.edu/content/how-make-readmetxt-file)


--------------------
CSE321_project1_alexkant_corrected_code.cpp:
--------------------
 Uses a thread to drive an LED with an on time of 2000ms and off time of 500ms every other time a button is pressed, and counts presses to determine whether or not a press is odd

----------
Variables Declared
----------
int oddPress: 0 on even presses, 1 on odd presses

int buttonHeld: a flag variable set to 1 when Button 1 is pressed

InterruptIn Button1: The button on PC_13

DigitalOut BlueLED: The LED on PB_7

----------
API and Built In Elements Used
----------
Thread: Allows for scheduling and priority managment, executes based on an event
  .start: starts execution of the Thread
  
  .get_state: shows the state of the thread, which we then print


DigitalOut: Sets a GPIO pin as output, and handles all of the initialization


InterruptIn: creates a reference to an interrupt with a variable name, and is triggered by a button

  .rise: trigger when the button is depressed

  .fall: trigger when the button is released


----------
Custom Functions
----------

void TimedLEDToggle:

Toggles the LED on for 500ms and off for 2000ms every other time Button 1 is pressed. 

Inputs: None

void HoldingButton:

Sets a flag variable to 1, indicating that Button 1 has been pressed.

Inputs: None

void CountPresses:

Determines whether the next press is even or odd using the oddPress variable, and resets the buttonHeld flag to 0

Inputs: None


