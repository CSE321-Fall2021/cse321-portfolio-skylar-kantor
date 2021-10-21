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
#include "PinNames.h"
#include "mbed.h"
#include "mbed_wait_api.h"


// functions to check which key on a collumn has been pressed and react
// appropriately
void one(void);
void two(void);
void three(void);
void A(void);

void timing(void);

// interrupt handlers for each collumn
InterruptIn oneToSeven(PB_8);  // D15
InterruptIn twoToZero(PB_9);   // D14
InterruptIn threeToNine(PA_5); // D13
InterruptIn AToD(PA_6);        // D12

Ticker tick;

int row = 0;

int totalTime = 60;
int timePos = 0;
int newTime = 0;
int firstDigit = 1;
int secondDigit, thirdDigit = 0;

int enteringTime = 0;
int timeRunning = 1;

char i = 0;
CSE321_LCD display(16, 2, LCD_5x10DOTS, PF_0, PF_1);

int main() {
 tick.attach(&timing, 1s);

  // set the interrupt handlers to trigger on the rising edge, when they key is
  // pressed
  oneToSeven.rise(&one);
  twoToZero.rise(&two);
  threeToNine.rise(&three);
  AToD.rise(&A);

  RCC->AHB2ENR |= 0x28; // enable ports D and F

  GPIOD->MODER &= ~(0x2 << 16);
  GPIOD->MODER |= (0x1 << 16); // set pin D8(Nucleo D1, white LED) as output

  GPIOD->MODER &= ~(0x2 << 18);
  GPIOD->MODER |= (0x1 << 18); // set pin D9(Nucleo D1, white LED) as output

  GPIOF->MODER &= ~(0x2 << 30);
  GPIOF->MODER |= (0x1 << 30); // set pin F15(Nucleo D2, yellow LED) as output

  display.begin(); // start the LCD
 //  display.print("test");
  // setup power supply for keypad row lines

  // Row 0, 1-A
  // Set pin F14 as an output
  GPIOF->MODER &= ~(0x2 << 28);
  GPIOF->MODER |= (0x1 << 28);

  // Row 1, 4-B
  // Set pin E11 as an output
  GPIOE->MODER &= ~(0x2 << 22);
  GPIOE->MODER |= ~(0x1 << 22);

  // Row 2, 7-C
  // Set pin E9 as an output
  GPIOE->MODER &= ~(0x2 << 18);
  GPIOE->MODER |= (0x1 << 18);

  // Row 3 *-D
  // Set pin F13 as an output
  GPIOF->MODER &= ~(0x2 << 26);
  GPIOF->MODER |= (0x1 << 26);

  while (true) {
      
    // turn off row 3 and power row 0
    GPIOF->ODR &= ~(0x1 << 13);
    GPIOF->ODR |= (0x1 << 14);
    wait_us(150); // wait to change values for bounce
    row = 1;      // set the row so we can determine which key was pressed

    // turn off row 0 and power row 1
    GPIOF->ODR &= ~(0x1 << 14);
    GPIOE->ODR |= (0x1 << 11);
    wait_us(150); // wait to change values for bounce
    row = 2;      // set the row so we can determine which key was pressed

    // turn off row 1 and power row 2
    GPIOE->ODR &= ~(0x1 << 11);
    GPIOE->ODR |= (0x1 << 9);
    wait_us(150); // wait to change values for bounce
    row = 3;      // set the row so we can determine which key was pressed

    // turn off row 2 and power row 3
    GPIOE->ODR &= ~(0x1 << 9);
    GPIOF->ODR |= (0x1 << 13);
    wait_us(150); // wait to change values for bounce
    row = 0;      // set the row so we can determine which key was pressed
  }
}

void timing(void) {
    //printf("Timing! %d\n", totalTime);
 if(timeRunning == 0){return;} //If the timer hasn't started, this function should not be running
    if (totalTime > 0) {
      display.clear();
      display.setCursor(0, 0);
      display.print("Time Remaining:");
      display.setCursor(0, 1);
      char remainingTime[4];
      remainingTime[0] = '0' + firstDigit;
      remainingTime[1] = ':';
      remainingTime[2] = '0' + secondDigit;
      remainingTime[3] = '0' + thirdDigit;
      display.print(remainingTime);
      printf("time remaining %s\n", remainingTime);
      totalTime--; //Take one second off the remaining time
      if (thirdDigit > 0)
        thirdDigit--; //If there's a digit in the seconds place, subtract one
      else if (secondDigit > 0) {
        thirdDigit = 9;
        secondDigit--; //Otherwise, carry 10 seconds from the 10-second place and subtract 1
      } else {
        firstDigit--;
        secondDigit = 5;
        thirdDigit = 9; //If the only digit is in the minutes place, subtract one there, carry that over to the 10-seconds place, and then carry 10 seconds to the seconds place, and subtract one
      }
    } else {
      display.clear();
      display.print("Time's Up");
      printf("Time's up\n");
      timeRunning = 0;
    }
}

void KeypressHandler(int col) {
  GPIOD->ODR |= (0x1 << 9);  // Turn on D9, the white LED
  thread_sleep_for(100);     // Wait so the user can see the flash
  GPIOD->ODR &= ~(0x1 << 9); // Turn off D9, the white LED

  if (col == 0) {
    if (row == 0) {
      newTime = 1;
    } else if (row == 1) {
      newTime = 4;
    } else if (row == 2) {
      newTime = 7;
    }
  }

  else if (col == 1) {
    if (row == 0) {
      newTime = 2;
    } else if (row == 1) {
      newTime = 5;
    } else if (row == 2) {
      newTime = 8;
    } else if (row == 3) {
      newTime = 0;
    }
  }

  else {
    if (row == 0) {
      newTime = 3;
    }
    if (row == 1) {
      newTime = 6;
    }
    if (row == 2) {
      newTime = 9;
    }

    if (timePos == 0) //If this is the first button the user pressed, change the digit in the minutes place, and add 60 * that digit to the total time
    {
      firstDigit = newTime;
      totalTime += newTime * 60;
    }
    else if (timePos == 1 && newTime < 6) //If this is the second button the user pressed, change the digit in the 10-second place and add 10 * that digit to the total time if the new digit is less than 6 to avoid nonsense times like 2:65
      {
      secondDigit = newTime;
      totalTime += newTime * 10;
      }
    else
    {
      thirdDigit = newTime; //If this is the third button the user pressed, change the digit in the seconds place and add that digit to the total time
      totalTime += newTime;
    }
    if (timePos < 3) //Times can only be 3 digits, so if the time is 2 or fewer, let the user edit the next digit with a button press
      timePos++;
    else {
      timePos = 0;
      //enteringTime = 0; //If the user has entered all 3 digits of the time, start at the beginning
    }
    
  }
  
}

void one(void) {
  if (enteringTime == 1)
    KeypressHandler(0); // A key was pressed, so flash the LED
}

void two(void) {
  if (enteringTime == 1)
    KeypressHandler(1); // A key was pressed, so flash the LED
}

void three(void) {
  if (enteringTime == 1)
    KeypressHandler(2); // A key was pressed, so flash the LED
}

void A(void) {
  if (row == 0) {
    // User pressed A, start the timer
    timeRunning = 1;
    
  } else if (row == 1) {
    // User pressed B, stop the timer
    timeRunning = 0;
    display.print("B");
  } else if (row == 2) {
    display.print("C");
  } else if (row == 3) {
    // User pressed D, show a prompt to input the time
    display.clear();
    display.print("Enter Time");
    enteringTime = 1;
  }
}


