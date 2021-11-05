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
-Counts up or down from any time less than or equal to 9min 59sec
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
Mbed OS Documentation (https://os.mbed.com/docs/mbed-os/v6.15/)
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

int row: Keeps track of which row of the keypad is being powered, since each collumn shares a pin

int totalTime: The total time being counted up to or down from
int timePos: Which digit of the time is being altered. Used to know which digit variable to change, and how much to change total time by
int newTime: The digit pressed by the user


int firstDigit: The digit in the minutes place on the LCD
int secondDigit:The digit in the ten seconds place on the LCD
int thirdDigit: The digit in the seconds place on the LCD

bool enteringTime: Determines whether or not the user is entering a new time
bool timeRunning: Determines whether or not the timer is counting

char remainingTime[4]: String representation of remaining time in m:ss format

bool countingDown: Determines the counting direction of the timer

CSE321_LCD display: The LCD object

----------
API and Built In Elements Used
----------


InterruptIn: creates a reference to an interrupt with a variable name, and is triggered by a button

  .rise: trigger when the button is depressed

wait_us: wait a certain nuber of microseconds

thread_sleep_for: wait a certain number of milliseconds
Ticker: Creates an object to call a function periodically after a certain time
EventQueue: Provides a safe way to call non interrupt-safe functions from an interrupt context


----------
Custom Functions
----------
void one(void): Handle keypresses in the first collumn of the keypad
void two(void): Handle keypresses in the second collumn of the keypad
void three(void): Handle keypresses in the third collumn of the keypad
void A(void): Handle keypresses in the fourth collumn of the keypad

void timing(void): Function that is called through the Ticker. Detects whether or not the total time has been reached, and displays the appropriate message on the LCD, and calls the apropriate helper function to display the remaining time on the LCD
void displayTime(void): Takes the integer representation of each digit and creates a string to display on the LCD
void timeEnter(void): Prints the time entry propmpt to the LCD
void KeypressHandler(int col, int row): Takes a row and collumn from each collumn handler ISR and calls the apropriate helper functions and updates global variables accordingly
void UpdateDigits (void): Changes the three integers for the display representation of the remaining time up or down depending on the counting direction

