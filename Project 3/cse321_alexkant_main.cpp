/**                                                                             
 *File: cse321_alexkant_project3_main.cpp                                       
 *Author Skylar Kantor (alexkant@buffalo.edu)                                   
 *Date: Fall 2021                                                               
 *Partner: None                                                                 
 *Course: CSE 321                                                               
 *Assignent: Project 3                                                         
 *Inputs: DHT11 Humidity sensor                                                                
 *Outputs: 7-segment display and LEDs                                                    
 *File Summary: Communicates with the DHT11 humidity and temperature sensor to find the dewpoint or whether the temperature is near freezing, calculates a safe speed limit, and displays that on the 7-segment display
 */  

 #include "mbed.h"
#include <cmath>
#include "DHT.h"
#include "DigitDisplay.h"


Thread readAndCalculate;
Thread findSpeedLimitAndDisplay;

bool is_dewy(int temp, int hum);
int speed (int temp, int hum);

int postedLimit = 55;

bool *atDewPoint;
int *speedLimit;
int *temperature;
int *humidity;

Ticker tick;

int main()
{


    return 0;
}










bool is_dewy(int temp, int hum)
{
    double dewPoint = temp-((100-hum)/5);
    if(temp <= dewPoint)
    return true;
    else
     return false;
}

int speed(int temp, int hum, int *posted)
{
    double speed = *posted;

    //If the temperature is not near freezing (more than 5 degrees above 0C), and the road is not wet, keep the posted limit
    if(!is_dewy(temp, hum) && temp >= 5)
        return speed;

    else if(is_dewy(temp, hum) && temp >= 5)
        return round(speed/3);
    else if(!is_dewy(temp, hum) && temp < 5)
        return round(speed * 0.8);
    else
     return round(speed/8);
}