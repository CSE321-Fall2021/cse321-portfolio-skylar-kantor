/**
*File: main.cpp
*Author Skylar Kantor (alexkant@buffalo.edu)
*Date: Fall 2021
*Partner: None
*Course: CSE 321
*Assignent: Project 2
*Inputs: None
*Outputs: Serial and external LED
*Sources: MBED OS blinky baremetal
*File Summary: outputs to serial and blinks LED
*/

#include "PinNames.h"
#include "mbed.h"


//DigitalOut light(PF_15); //D2 on the nucleo

int main()
{
    RCC->AHB2ENR |= 0b00000000000000000000000000100000;
    
    GPIOF->MODER |= 0b10000000000000000000000000000000;
    
    
    printf("Skylar Kantor\n"); //serial output 

    while (true)
    {
    
    GPIOF->ODR |= 0b00000000000000001000000000000000;
    GPIOF->ODR &= ~(0b00000000000000001000000000000000);
    
    thread_sleep_for(500);
    }
}
