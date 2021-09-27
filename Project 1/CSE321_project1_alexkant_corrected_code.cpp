/**
*File: CSE321_project1_alexkant_corrected_code.cpp
*Author Skylar Kantor (alexkant@buffalo.edu)
*Date: Fall 2021
*Partner: None
*Course: CSE 321
*Assignent: Project 1
*Inputs: Button1 (PC_13)
*Outputs: LED2 (PB_7)
*Constraints: None
*Modules/Subroutines: 
TimedLEDToggle:
	Toggles the LED on for 500ms and off for 2000ms every other time Button 1 is pressed. 
	Inputs:
		None
HoldingButton:
    Sets a flag variable to 1, indicating that Button 1 has been pressed.
    Inputs: 
        None
*Sources: Provided code
*File Summary: Create a thread to drive an LED to have an on time of 2000ms and off time of 500ms, every other time button 1 is pressed
*/
#include "mbed.h"



Thread controller;


void TimedLEDToggle(); 
void HoldingButton();
void CountPresses();

DigitalOut BlueLED(LED2); // establish blue led as an output
InterruptIn Button1(BUTTON1); 

int oddPress = 0; 
int buttonHeld = 0;

int main() {
    // start the allowed execution of the thread
    printf("----------------START----------------\n");
    printf("Starting state of thread: %d\n", controller.get_state());
    controller.start(TimedLEDToggle);
    printf("State of thread right after start: %d\n", controller.get_state());
    //On the rising edge (when pressed), set a flag that indicates the button is down
    Button1.rise(HoldingButton);
    //On the falling edge (release), determine whether the next press will be odd or even
    Button1.fall(CountPresses);
    return 0;
}

//make the handler
void TimedLEDToggle() {
    while (true) {
        //Only do this every other press, starting with the first
        if(oddPress == 0){
            BlueLED = !BlueLED;
            printf("Toggled Blue LED on"); 
            thread_sleep_for(2000); //Thread_sleep is a time delay function, causes a 2000 unit delay
            BlueLED = !BlueLED;
            printf("Toggled Blue LED off"); 
            thread_sleep_for(500); //Thread_sleep is a time delay function, causes a 500 unit delay
        }
    }
}

void HoldingButton() {
    // togle the state of the thread
    buttonHeld = 1;
}

void CountPresses() {
    if (buttonHeld == 1){
        //determine whether the next press will be odd (1) or even (0)
        oddPress++; 
        oddPress %= 2;

        //reset the button flag for the next press
        buttonHeld = 0;
    }
}
