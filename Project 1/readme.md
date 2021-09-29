-------------------
About
-------------------
Project Description: 

Contribitor List:
Skylar Kantor (alexkant@buffalo.edu)


--------------------
Features
--------------------



--------------------
Required Materials
--------------------
-Nucleo L4R5ZI

--------------------
Resources and References
--------------------


--------------------
Getting Started
--------------------
This section will describe the general setup

--------------------
p1_code_provided.cpp:
--------------------
 
This file has lots of things. There is a os typle tool used to create periodic events with a periepherial. The name of the file and the contents in here will be entirely updated.
 
This is totally not bare metal since there are some cool tools used. Those tools instantiate with a finite reference but gots their unique object created. 


----------
Variables Declared
----------
int oddPress: 0 on even presses, 1 on odd presses

int buttonHeld: a flag variable set to 1 when Button 1 is pressed

----------
API and Built In Elements Used
----------
Thread: Allows for scheduling and priority managment, executes based on an event

DigitalOut: Sets a GPIO pin as output, and handles all of the initialization

InterruptIn: creates a reference to an interrupt with a variable name, and is triggered by a button

    .rise: trigger when the button is depressed

    .fall: trigger when the button is released


----------
Custom Functions
----------

void TimedLEDToggle:

	Toggles the LED on for 500ms and off for 2000ms every other time Button 1 is pressed. 

	Inputs:

		None
void HoldingButton:

    Sets a flag variable to 1, indicating that Button 1 has been pressed.

    Inputs: 

        None

void CountPresses:

    Determines whether the next press is even or odd using the oddPress variable, and resets the buttonHeld flag to 0

    Inputs:

        None


