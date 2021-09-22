
#include "mbed.h"

// Create a thread to drive an LED to have an on time of 2000ms and off time of 500ms

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
    //On the rising edge (when pressed), call HoldingButton
    Button1.rise(HoldingButton);
    //On the falling edge (release), call CountPresses
    Button1.fall(CountPresses);
    return 0;
}

void TimedLEDToggle() {
    while (true) {
        //Only do this every other press, starting with the first
        if(oddPress == 0){
            BlueLED = !BlueLED;
            printf("Toggled Blue LED on");
            thread_sleep_for(2000);
            BlueLED = !BlueLED;
            printf("Toggled Blue LED off"); 
            thread_sleep_for(500); 
        }
    }
}

void HoldingButton() {
	buttonHeld = 1;
}

void CountPresses() {
    if (buttonHeld == 1){
        oddPress++; 
        oddPress %= 2;
        buttonHeld = 0;
    }
}
