/**
 *File: cse321_alexkant_project2_main.cpp
 *Author Skylar Kantor (alexkant@buffalo.edu)
 *Date: Fall 2021
 *Partner: None
 *Course: CSE 321
 *Assignent: Project 2
 *Inputs: Keypad
 *Outputs: LCD and LED
 *File Summary: A timer with an LCD screen and LED lights. Contains handlers for
 *keypresses and the timer code itself
 */

#include "1802.h"
#include "DigitalOut.h"
#include "InterruptIn.h"
#include "PinNamesTypes.h"
#include "mbed.h"
#include "mbed_events.h"
#include "mbed_wait_api.h"

// Interrup handlers     to check which key on a collumn has been pressed and react
// appropriately
void one(void);
void two(void);
void three(void);
void A(void);

void timing(void);
void displayTime(void);

// interrupt Objects for each collumn
InterruptIn oneToSeven(PA_6, PullDown);   //D12, PA_6
InterruptIn twoToZero(PA_5, PullDown);    //D13, PA_5
InterruptIn threeToNine(PB_9, PullDown); //D14, PB_9
InterruptIn AToD(PB_8, PullDown);        //D15, PB_8

// EventQueue so we can work with non interrupt safe functions
EventQueue q1(32 * EVENTS_EVENT_SIZE);

Ticker tick; //Ticker for timing so we can trigger an event every second

int row = 0;

int totalTime = 0;
int timePos = 0;
int newTime = 0;

int firstDigit = 0;
int secondDigit = 0;
int thirdDigit = 0;

bool enteringTime = false;
bool timeRunning = false;

char remainingTime[4];

CSE321_LCD display(16, 2, LCD_5x10DOTS, PF_0, PF_1);

int main() {

  // set the interrupt handlers to trigger on the rising edge, when they key is
  // pressed
  oneToSeven.rise(&one);
  twoToZero.rise(&two);
  threeToNine.rise(&three);
  AToD.rise(&A);

  RCC->AHB2ENR |= (0x1 << 3); // Enable port D
  RCC->AHB2ENR |= (0x1 << 5); // Enable port F
  RCC->AHB2ENR |= (0x1 << 4); // Enable port E

  GPIOD->MODER &= ~(0x2 << 16);
  GPIOD->MODER |= (0x1 << 16); // set pin D8(Nucleo D1, white LED) as output

  GPIOD->MODER &= ~(0x2 << 18);
  GPIOD->MODER |= (0x1 << 18); // set pin D9(Nucleo D0, red LED) as output

  display.begin(); // start the LCD

  // setup power supply for keypad row lines
  // Row 0, 1-A
  // Set pin F14 (Nucleo D4) as an output
  GPIOF->MODER &= ~(0x2 << 28);
  GPIOF->MODER |= (0x1 << 28);

  // Row 1, 4-B
  // Set pin E11 (Nucleo D5) as an output
  GPIOE->MODER &= ~(0x2 << 22);
  GPIOE->MODER |= ~(0x1 << 22);

  // Row 2, 7-C
  // Set pin E9 (Nucleo D6) as an output
  GPIOE->MODER &= ~(0x2 << 18);
  GPIOE->MODER |= (0x1 << 18);

  // Row 3 *-D
  // Set pin F13 (Nucleo D7) as an output
  GPIOF->MODER &= ~(0x2 << 26);
  GPIOF->MODER |= (0x1 << 26);

  while (true) {
    // set the row to 0
    row = 0;
    // Turn off row 3 (F13)
    GPIOF->ODR &= ~(0x1 << 13);
    // turn on row 0 (F14)
    GPIOF->ODR |= (0x1 << 14);
    wait_us(500);

    // set the row to 1
    row = 1;
    // turn off row 0 (F14)
    GPIOF->ODR &= ~(0x1 << 14);
    // turn on row 1 (E11)
    GPIOE->ODR |= (0x1 <<11);
    wait_us(1000);

    // set row to 2
    row = 2;
    // turn off row 1 (E11)
    GPIOE->ODR &= ~(0x1 << 11);
    // turn on row 2 (E9)
    GPIOE->ODR |= (0x1 << 9);
    wait_us(500);

    // set row to 3
    row = 3;
    // turn off row 2 (E9)
    GPIOE->ODR &= ~(0x1 << 9);
    // turn on row 3(F13)
    GPIOF->ODR |= (0x1 << 13);
    wait_us(500);

    if (enteringTime) {
      // So that the user can see what time they've entered, display it here to
      // them as they type
      display.setCursor(0, 1);
      displayTime();
    }
    q1.dispatch_once(); // Dispatch any items in the eventQueue
  }
}

void displayRunningTime() {
  if (thirdDigit > 0)
    thirdDigit--; // If there's a digit in the seconds place, subtract one from
                  // that
  else if (secondDigit > 0) {
    thirdDigit = 9;
    secondDigit--; // Otherwise, carry 10 seconds from the 10-second place and
                   // subtract 1
  } else {
    firstDigit--;
    secondDigit = 5;
    thirdDigit = 9; // If the only digit is in the minutes place, subtract one
                    // there, carry that over to the 10-seconds place, and then
                    // carry 10 seconds to the seconds place, and subtract one
  }
  display.setCursor(0, 1);
  displayTime(); // print out the string representation of
                 // the 3 digits of time we have
}

void timing(void) {
  if (!timeRunning)
    return;
  display.clear();
  display.setCursor(0, 0);
  if (totalTime > 0) {
    display.print("Time Remaining:");
    totalTime--;
  } else {
    display.print("Time's Up");
    GPIOD->ODR |= 0x1 << 8; // Turn on D8, the white LED
    GPIOD->ODR |= 0x1 << 9; // Turn on D9, the red LED
    return;
  }
  displayRunningTime();
}

void timeEnter() {
  display.setCursor(0, 0);
  display.clear();
  display.print("Enter time~");
}

void KeypressHandler(int col, int row) {
 
 wait_us(900000);
 newTime = 0;
 printf("Detected (%d,%d)\n", row, col);
  GPIOD->ODR |= (0x1 << 9);  // Turn on D9, the red LED
  wait_us(8000);             // Wait so the user can see the flash
  GPIOD->ODR &= ~(0x1 << 9); // Turn off D9, the red LED

  // Each of the collumn and row combinations correspond to a certain key on the
  // keypad, listed in the diagram below. For our purposes, the * and # keys are
  // inoperative and don't do anything
  /*    0 1 2 3
   *  -----------
   *0 | 1 2 3 A |
   *1 | 4 5 6 B |
   *2 | 7 8 9 C |
   *3 | * 0 # D |
   *  -----------*/

  if (row == 0) {
    if (col == 0) {
      newTime = 1;
    } else if (col == 1) {
      newTime = 2;
    } else if (col == 2) {
      newTime = 3;
    } else if (col == 3) {
      enteringTime = false;
      timeRunning = true;
      tick.attach(q1.event(timing), 1s);
      timePos--;
    }

  }

if (row == 1) {
    if (col == 0) {
      newTime = 4;
    } else if (col == 1) {
      newTime = 5;
    } else if (col == 2) {
      newTime = 6;
    } else if (col == 3) {
      timeRunning = false;
      // User pressed B, so stop the timer
      tick.detach();
      timePos--;
    }
  }

  if (row == 2) {
    if (col == 0) {
      newTime = 7;
    } else if (col == 1) {
      newTime = 8;
    } else if (col == 2) {
      newTime = 9;
    } else if (col == 3) {
      // C is inoperative in normal use, but we should still detect it for the
      // bonus
      timePos--;
    }

  }

  else if (row == 3) {
    if (col == 1) {
      newTime = 0;
    }

    else if (col == 3) {
      tick.detach();
      GPIOD->ODR &= ~(0x1 << 8); // Turn off D8, the white LED
      GPIOD->ODR &= ~(0x1 << 9); // Turn off D9, the red LED
      firstDigit = 0;
      secondDigit = 0;
      thirdDigit = 0;
      totalTime = 0;
      timePos = 0;
      enteringTime = true;
      timeRunning = false;
      timePos--;
      q1.call(timeEnter);
    }
  }

  if (enteringTime) {
    if (timePos ==
        0) // If this is the first button the user pressed, change the digit in
           // the minutes place, and add 60 * that digit to the total time
    {
      firstDigit = newTime;
      totalTime += newTime * 60;
    } else if (timePos == 1 && newTime < 6) // If this is the second button the user pressed,
                      // change the digit in the 10-second place and add 10
                      // * that digit to the total time if the new digit is
                      // less than 6 to avoid nonsense times like 2:65
    {
      secondDigit = newTime;
      totalTime += newTime * 10;
    } else {
      thirdDigit = newTime; // If this is the third button the user pressed,
                            // change the digit in the seconds place and add
                            // that digit to the total time
      totalTime += newTime;
    }
    if (timePos < 3) // Times can only be 3 digits, so if the time is 2 or fewer,
           // let the user edit the next digit with a button press
      timePos++;
    else {
      timePos = 0; // If the user has entered all 3 digits of the time,
                   // start at the beginning
    }
  }
}

void one(void) {
  KeypressHandler(0, row); // A key was pressed, so let's figure out which one
                           // and what to do about it
}

void two(void) {
  KeypressHandler(1, row); // A key was pressed, so flash the LED
}

void three(void) {
  KeypressHandler(2, row); // A key was pressed, so flash the LED
}

void A(void) {
  KeypressHandler(3, row); // A key was pressed, so flash the LED
}

void displayTime() {

  remainingTime[0] = '0' + firstDigit; //'0' is the character representation of the number 0
                        // and since the ASCII values for numbers 0-9 are in
                        // order we can just add the digit to it
  remainingTime[1] = ':';
  remainingTime[2] = '0' + secondDigit;
  remainingTime[3] = '0' + thirdDigit;
  display.print(remainingTime); // print out the string representation of the
                                // 3 digits of time we have
}