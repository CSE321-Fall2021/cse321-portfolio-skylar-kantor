/**
 *File: cse321_alexkant_project2_main.cpp
 *Author Skylar Kantor (alexkant@buffalo.edu)
 *Date: Fall 2021
 *Partner: None
 *Course: CSE 321
 *Assignent: Project 2
 *Inputs: Keypad
 *Outputs: LCD and LED
 *File Summary: A timer with an LCD screen and LED lights. Contains handlers for keypresses and the timer code itself
 */

#include "1802.h"
#include "InterruptIn.h"
#include "PinNames.h"
#include "mbed.h"
#include "mbed_thread.h"
#include "mbed_wait_api.h"

//functions to check which key on a collumn has been pressed and react appropriately
void one(void);
void two(void);
void three(void);
void A(void);

//interrupt handlers for each collumn
InterruptIn oneToSeven(PB_8);  // D15
InterruptIn twoToZero(PB_9);   // D14
InterruptIn threeToNine(PA_5); // D13
InterruptIn AToD(PA_6);        // D12

int row = 0;

CSE321_LCD display(16, 2, LCD_5x10DOTS, PF_0, PF_1);

int main() 
{
  //set the interrupt handlers to trigger on the rising edge, when they key is pressed
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
  GPIOF->MODER |= (0x1 << 30);// set pin F15(Nucleo D2, yellow LED) as output 

  display.begin(); //start the LCD

  /// setup power supply for keypad row lines

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
    row = 1; //set the row so we can determine which key was pressed
    wait_us(150); //wait to change values for bounce

    // turn off row 0 and power row 1
    GPIOF->ODR &= ~(0x1 << 14);
    GPIOE->ODR |= (0x1 << 11);
    row = 2;//set the row so we can determine which key was pressed
    wait_us(150); //wait to change values for bounce


    // turn off row 1 and power row 2
    GPIOE->ODR &= ~(0x1 << 11);
    GPIOE->ODR |= (0x1 << 9);
    row = 3;//set the row so we can determine which key was pressed
    wait_us(150); //wait to change values for bounce


    // turn off row 2 and power row 3
    GPIOE->ODR &= ~(0x1 << 9);
    GPIOF->ODR |= (0x1 << 13);
    row = 0;//set the row so we can determine which key was pressed
    wait_us(150); //wait to change values for bounce

  }
}

void LEDFlash(void) 
{
     GPIOD->ODR |= (0x1 << 9); // Turn on D9, the white LED
     thread_sleep_for(100); //Wait so the user can see the flash
     GPIOD->ODR &= ~(0x1 << 9); // Turn off D9, the white LED
}
void one(void) 
{
    LEDFlash(); //A key was pressed, so flash the LED
    if (row == 0) 
    {
        display.print("1");
    } 
    else if (row == 1) 
    {
        display.print("4");
    } 
    else if (row == 2) 
    {
        display.print("7");
    } 
    else if (row == 3) 
    {
        display.print("*");
    }
}

void two(void) 
{
    LEDFlash();//A key was pressed, so flash the LED
    if (row == 0) 
    {
        display.print("2");
    }
    else if (row == 1) 
    {
        display.print("5");
    }
    else if (row == 2) 
    {
        display.print("8");
    }
    else if (row == 3) 
    {
        display.print("0");
    }
}

void three(void) 
{
    LEDFlash();//A key was pressed, so flash the LED
    if (row == 0) 
    {
        display.print("3");
    }
    else if (row == 1)
    {
        display.print("6");
    }
    else if (row == 2) 
    {
        display.print("9");
    }
    else if (row == 3)
    {
        display.print("#");
    }
}

void A(void) {
    //No value on letter key presses, so don't flash the LED
    if (row == 0) 
    {
        //User pressed A, start the timer
        display.print("Time Remaining: ");
    }
    else if (row == 1)
    {
        //User pressed B, stop the timer
        display.print("B");
    } 
    else if (row == 2)
    {
        display.print("C");
    }
    else if (row == 3)
    {
        //User pressed D, show a prompt to input the time
        display.print("Enter Time");
    }
}
