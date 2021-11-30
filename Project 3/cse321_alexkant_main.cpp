/**
 *File: cse321_alexkant_project3_main.cpp
 *Author Skylar Kantor (alexkant@buffalo.edu)
 *Date: Fall 2021
 *Partner: None
 *Course: CSE 321
 *Assignent: Project 3
 *Inputs: DHT11 Humidity sensor
 *Outputs: 7-segment display and LEDs
 *File Summary: Communicates with the DHT11 humidity and temperature sensor to
 *find the dewpoint or whether the temperature is near freezing, calculates a
 *safe speed limit, and displays that on the 7-segment display
 */

#include "DHT.h"
#include "DigitDisplay.h"
#include "PinNames.h"
#include "Watchdog.h"
#include "mbed.h"
#include <cmath>

Thread readAndCalculate;
Thread findSpeedLimitAndDisplay;

void is_dewy(int temp, int hum);
void speed(int temp, int hum);

DHT11 sensor(PB_0);

DigitDisplay sign(PA_0, PA_1);

int *postedLimit;

bool *atDewPoint;
int *speedLimit;
int *temperature;
int *humidity;

Ticker tick;

int main() {
  Watchdog::get_instance().start();
  *postedLimit = 55;
  sign.setColon(false);
  sign.setBrightness(7);
  return 0;
}

void is_dewy() {
  double dewPoint = *(double *)temperature - ((100 - *(double *)humidity) / 5);
  if (*temperature <= dewPoint)
    *atDewPoint = true;
  else
    *atDewPoint = false;
  Watchdog::get_instance().kick();
}

void speed() {
  double speed = *postedLimit;

  // If the temperature is not near freezing (more than 5 degrees above 0C), and
  // the road is not wet, keep the posted limit Or, if there is an issue with the
  // DHT 11, keep the posted limit
  if ((!*atDewPoint && *temperature >= 5) || *humidity == -1)
    *speedLimit = speed;

  else if (*atDewPoint && *temperature >= 5)
    *speedLimit = round(speed / 3);
  else if (!*atDewPoint && *temperature < 5)
    *speedLimit = round(speed * 0.8);
  else
    *speedLimit = round(speed / 8);
  Watchdog::get_instance().kick();
}

void getTempAndHumidity() {
  if (sensor.read() == 0) {
    *temperature = sensor.getCelsius();
    *humidity = sensor.getHumidity();
  } else {
    *temperature = 999;
    *humidity = -1;
  }
  Watchdog::get_instance().kick();
}

void displaySpeedLimit() {
  sign.clear();
  sign.write(*speedLimit);
  if (abs(*speedLimit - *postedLimit) > 5) {
    // Flash connected LEDs
  }
  Watchdog::get_instance().kick();
}