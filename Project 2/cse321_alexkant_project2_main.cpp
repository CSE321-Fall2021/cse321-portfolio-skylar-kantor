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
#include "InterruptIn.h"
#include "mbed.h"
#include "mbed_events.h"

// Interrupt handlers to check which key on a collumn has been pressed and react
// appropriately
void one(void);
void two(void);
void three(void);
void A(void);

//Helper functions for updating the time on the display
void timing(void);
void displayTime(void);
void UpdateDigits (void);

// interrupt objects for each collumn
InterruptIn oneToSeven(PA_6, PullDown);  // D12, Pin 5
InterruptIn twoToZero(PA_5, PullDown);   // D13, Pin 6
InterruptIn threeToNine(PB_9, PullDown); // D14 Pin 7
InterruptIn AToD(PB_8, PullDown);        // D15, Pin 8

// EventQueue so we can work with non interrupt safe functions
EventQueue q1(32 * EVENTS_EVENT_SIZE);

Ticker tick; // Ticker for timing so we can trigger an event every second

int row = 0;

int totalTime = 0;
int timePos = 0;
int newTime = 0;

// Integer values of each digit for string representation
int firstDigit = 0;
int secondDigit = 0;
int thirdDigit = 0;

bool enteringTime = false;
bool timeRunning = false;

char remainingTime[4]; // String representation of remaining time in m:ss format

bool countingDown = true; // For the bonus, make sure to detect which direction
                          // the user wants to count

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
  display.print("Timer On. Press D to enter time");

  // setup power supply for keypad row lines
  // Row 0, 1-A
  // Set pin F14 (Nucleo D4) as an output (Pin 1 on keypad)
  GPIOF->MODER &= ~(0x2 << 28);
  GPIOF->MODER |= (0x1 << 28);

  // Row 1, 4-B
  // Set pin E11 (Nucleo D5) as an output (Pin 2 on keypad)
  GPIOE->MODER &= ~(0x2 << 22);
  GPIOE->MODER |= ~(0x1 << 22);

  // Row 2, 7-C
  // Set pin E9 (Nucleo D6) as an output (Pin 3 on keypad)
  GPIOE->MODER &= ~(0x2 << 18);
  GPIOE->MODER |= (0x1 << 18);

  // Row 3 *-D
  // Set pin F13 (Nucleo D7) as an output (Pin 4 on keypad)
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
    GPIOE->ODR |= (0x1 << 11);
    wait_us(500);

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
      display.setCursor(0, 1); // Set the cursor to the second line because
                               // we'll have some text on the first line
      displayTime();
    }
    q1.dispatch_once(); // Dispatch any items in the eventQueue
  }
}

void UpdateDigits() {
  if (countingDown) {
    if (thirdDigit > 0)
      thirdDigit--; // If there's a digit in the seconds place, subtract one
                    // from that
    else if (secondDigit > 0) {
      thirdDigit = 9;
      secondDigit--; // Otherwise, carry 10 seconds from the 10-second place and
                     // subtract 1
    } else {
      firstDigit--;
      secondDigit = 5;
      thirdDigit =
          9; // If the only digit is in the minutes place, subtract one
             // there, carry that over to the 10-seconds place, and then
             // carry 10 seconds to the seconds place, and subtract one
    }
  } else {
    if (thirdDigit < 9)
      thirdDigit++; //Since after 9 seconds we add to the ten seconds place, only increase until the third digit is 9
    else if (secondDigit < 5) {
      thirdDigit = 0; //Then, after the thrid digit is 9, set it back to 0, (ex 1:09 --> 1:10)
      secondDigit++; //And add to the tens place, until the digit in the ten seconds place is 5. 
                    //When there is a 9 in the seconds place, and a 5 in the 10 seconds place (ex 1:59), then
    } else {
      thirdDigit = 0; //Set the seconds and ten seconds places to 0
      secondDigit = 0;
      firstDigit++; //And add one to the minutes place (ex 1:59 --> 2:00)
    }
  }
  display.setCursor(0, 1); // Set the cursor to the second line because we'll
                           // have some text on the first line
  displayTime(); // print out the string representation of
                 // the 3 digits of time we have
}

void timing(void) {
  if (!timeRunning)
    return; // If we're not displaying the time to the user, this function
            // should not run because other information is on-screen
  display.clear();
  display.setCursor(0,
                    0); // Set the cursor to the top so the text prints properly
  if (totalTime > 0) {
    if (countingDown)
      display.print("Time Remaining:");
    else
      display.print("Time Elapsed");
    totalTime--;
  } else {
    if (countingDown)
      display.print("Time's Up");
    else
      display.print("Time Reached");
    GPIOD->ODR |= 0x1 << 8; // Turn on D8, the white LED
    GPIOD->ODR |= 0x1 << 9; // Turn on D9, the red LED
    return;
  }
  UpdateDigits();
}

void timeEnter() {
  display.setCursor(
      0, 0);       // Set the cursor to the top so the prompt prints properly
  display.clear(); // Clear out whatever else might be on the screen
  display.print("Enter time~");
}

void KeypressHandler(int col, int row) {
  wait_us(900000); // Wait until bounce has settled
  newTime = 0; // Clear out whatever might be in newTime from the last keypress
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
      tick.attach(q1.event(timing), 1s); // Trigger a call to timing every
                                         // second
      timePos--; // Since this isn't a number, don't increase the position in
                 // the time entry
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
      tick.detach(); // Stop calling the timing function
      timePos--; // Since this isn't a number, don't increase the position in
                 // the time entry
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
      timePos--; // Since this isn't a number, don't increase the position in
                 // the time entry
    }

  }

  else if (row == 3) {
    if (col == 1) {
      newTime = 0;
    }

    else if (col == 3) {
      // This is D, so user is entering time
      tick.detach();             // Stop calling the timing function
      GPIOD->ODR &= ~(0x1 << 8); // Turn off D8, the white LED
      GPIOD->ODR &= ~(0x1 << 9); // Turn off D9, the red LED
      // User wants to enter time, so clear all values from the previous entry
      firstDigit = 0;
      secondDigit = 0;
      thirdDigit = 0;
      totalTime = 0;
      timePos = 0;
      enteringTime = true;
      timeRunning = false;
      timePos--; // Since this isn't a number, don't increase the position in
                 // the time entry
      q1.call(timeEnter); // display the prompt
    }
  }

  if (enteringTime) {
    if (timePos ==
        0) // If this is the first button the user pressed, change the digit in
           // the minutes place, and add 60 * that digit to the total time
    {
      firstDigit = newTime;
      totalTime +=
          newTime * 60; // The first digit is the minute, or 60-second place
    } else if (timePos == 1 &&
               newTime <
                   6) // If this is the second button the user pressed,
                      // change the digit in the 10-second place and add 10
                      // * that digit to the total time if the new digit is
                      // less than 6 to avoid nonsense times like 2:65
    {
      secondDigit = newTime;
      totalTime += newTime * 10; // The second digit is the 10-second place
    } else {
      thirdDigit = newTime; // If this is the third button the user pressed,
                            // change the digit in the seconds place and add
                            // that digit to the total time
      totalTime += newTime; // the third digit is the seconds place
    }
    if (timePos <
        3) // Times can only be 3 digits, so if the time is 2 or fewer,
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
  KeypressHandler(1, row); // A key was pressed, so let's figure out which one
                           // and what to do about it
}

void three(void) {
  KeypressHandler(2, row); // A key was pressed, so let's figure out which one
                           // and what to do about it
}

void A(void) {
  KeypressHandler(3, row); // A key was pressed, so let's figure out which one
                           // and what to do about it
}

void displayTime() {

  remainingTime[0] =
      '0' + firstDigit; //'0' is the character representation of the number 0
                        // and since the ASCII values for numbers 0-9 are in
                        // order we can just add the digit to it
  remainingTime[1] = ':';
  remainingTime[2] = '0' + secondDigit;
  remainingTime[3] = '0' + thirdDigit;
  display.print(remainingTime); // print out the string representation of the
                                // 3 digits of time we have
}