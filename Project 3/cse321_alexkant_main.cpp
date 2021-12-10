/**
 *File: cse321_alexkant_project3_main.cpp
 *Author Skylar Kantor (alexkant@buffalo.edu)
 *Date: Fall 2021
 *Partner: None
 *Course: CSE 321
 *Assignent: Project 3
 *Inputs: DHT11 Humidity sensor
 *Outputs: 7-segment display and LEDs
 *References  1. D. Driving, “Driving in wet conditions,” Defensive Driving,
 *03-Aug-2017. [Online]. Available:
 *https://www.defensivedriving.com/blog/driving-in-wet-road-conditions/.
 *[Accessed: 10-Dec-2021].
 *2. M. G. Lawrence, “The relationship between relative humidity and the
 *dewpoint temperature in moist air: A simple conversion and applications,”
 *Bulletin of the American Meteorological Society, vol. 86, no. 2, pp. 225–234,
 *2005. File Summary: Communicates with the DHT11 humidity and temperature
 *sensor to find the dewpoint or whether the temperature is near freezing,
 *calculates a safe speed limit, and displays that on the 7-segment display
 *
 */

#include "DHT.h"
#include "DigitDisplay.h"
#include "EventQueue.h"
#include "Mutex.h"
#include "PinNames.h"
#include "ThisThread.h"
#include "Watchdog.h"
#include "mbed.h"
#include <cmath>
#include <cstdlib>

Thread readSensor;
Thread findSpeedLimit;
Thread updateDisplay;
Thread statusLED;

EventQueue displayQueue(32 * EVENTS_EVENT_SIZE);
EventQueue speedQueue(32 * EVENTS_EVENT_SIZE);
EventQueue sensorQueue(32 * EVENTS_EVENT_SIZE);

bool is_dewy();
void speed();
void getTempAndHumidity();
void displaySpeedLimit();
void status();

void dispatchTemp();
void dispatchDisplay();
void dispatchSpeed();

DHT11 sensor(PA_5);

DigitDisplay sign(PA_6, PA_7);

int postedLimit;

int speedLimit;
int temperature;
int humidity;

int dewDiff;

int errorNum;

Mutex tempHumLock;
Mutex speedLimitLock;

Ticker speedTick;
Ticker displayTick;
Ticker sensorTick;

int main() {

  // Schematic Note:PB8 = D15
  // Schematic Note: P9 = D14
  // start the watchdog with a two second timeout, the time between DHT11 reads
  Watchdog::get_instance().start(2000);

  // In normal conditions, the speed limit on this particular section of road
  // will be 55MPH
  postedLimit = 55;
  speedLimit = postedLimit;

  // Since only 2 digits of the display are used, the colon is unnecessary and
  // distracting, so turn it off
  sign.setColon(false);

  // Set the brightness to the highest setting, to ensure the sign is readable
  sign.setBrightness(7);

  // Get initial temperature and humidity values, in case somehow the speed
  // limit calculation thread still gets the lock first
  getTempAndHumidity();

  // Attach helper functions that add a call to the display, calculation and
  // sensor read functions to their tickers So that they trigger every two
  // seconds (minimum time between DHT11 reads)
  sensorTick.attach(&dispatchTemp, 2s);
  speedTick.attach(&dispatchSpeed, 2s);
  displayTick.attach(&dispatchDisplay, 2s);

  // Start the thread that checks the status of the DHT11
  statusLED.start(status);

  // Turn on the ports being used for the status LEDs (B and C)
  RCC->AHB2ENR |= (0x1 << 1);
  RCC->AHB2ENR |= (0x1 << 2);

  // Set the status LED pins to output mode (PC7, PB7, and PB14)
  // PC7
  GPIOC->MODER |= (0x1 << 14);
  GPIOC->MODER &= ~(0x2 << 14);
  // PB7
  GPIOB->MODER |= (0x1 << 14);
  GPIOB->MODER &= ~(0x2 << 14);
  // PB14
  GPIOB->MODER |= (0x1 << 28);
  GPIOB->MODER &= ~(0x2 << 28);

  // Set two other pins to output mode for the flashing LEDs around the display
  // PB8
  GPIOB->MODER |= (0x1 << 16);
  GPIOB->MODER &= ~(0x2 << 16);
  // PB9
  GPIOB->MODER |= (0x1 << 18);
  GPIOB->MODER &= ~(0x2 << 18);

  // always dispatch any events in the queues
  sensorQueue.dispatch_forever();
  speedQueue.dispatch_forever();
  displayQueue.dispatch_forever();

  return 0;
}

bool is_dewy() {
  // Since we need to access the temperature and humidity, lock the mutex to
  // safely read the data
  while (!tempHumLock.trylock()) {
  }
  dewDiff = 0;
  // Calculate the dew point based on the temperature and the humidity [2]
  double dewPoint = (double)temperature - ((100 - (double)humidity) / 5);
  // If the outside temperature is at or below the dew point, return true so the
  // calling function knows there could be water on the road
  if (temperature <= dewPoint)
    return true;
  else {
    // Otherwise, calculate a difference between the dew point and the actual
    // temperature, so that the calling function can determine if it still Is
    // neccessary to reduce the speed limit
    dewDiff = round(temperature - dewPoint);
    return false;
  }

  // Now that we've finished using the data, unlock the mutex so that other
  // threads can use the data
  tempHumLock.unlock();
  // This cycle of the thread is finished, so kick the watchdog to reset the
  // timer and make sure the system continues to run without a reset
  Watchdog::get_instance().kick();
}

void speed() {
  // Since we need the temperature and speed limit data, lock both mutexes to
  // safely read and write that data
  while (!tempHumLock.trylock() && !speedLimitLock.trylock()) {
  }
  double speed = postedLimit;

  // If at the dewpoint, use postedlimit/3
  if (is_dewy()) {
    speedLimit = round(speed / 3);

    // If it's near freezing, there may be ice, so decrease the speed limit
    // slightly more
    if (temperature <= 5) {
      speedLimit -= 5;
    }
  }
  // For large differences between dewpoint and current temperature, use posted
  // limit
  else if (dewDiff >= 10) {
    speedLimit = postedLimit;

  }

  // for all in between use the equation speedLimit
  // = 3.7(|teperature-dewPoint|)+(55/3).
  // This equation produces a speed limit of 55/3  [1] at the dewpoint, and a
  // speed limit of 55mph at a 10 degrees above the dewpoint, so it should
  // suffice for this purpose
  else {

    speedLimit = round(3.7 * (dewDiff) + (55.0 / 3.0));

    // If it's near freezing, even though the chance of precipitation is low
    // there could be snow or ice melting, making the road wet Since the
    // current implementation doesn't have a way to tell what time of year it
    // is, using slightly above freezing temperatures should be sufficent
    // evidence of it being near enough to winter that this is a concern So if
    // the temperature is near freezing, even without precipitation, lower the
    // speed limit slightly
    if (temperature <= 3)
      speedLimit -= 5;
  }

  // If the DHT11 has an issue, default back to the posted limit
  // The getTempAndHumidity function will notify us that there is an issue by
  // setting humidity to -1 That function will also set the necessary error
  // information for the staus thread to display on the indicator panel
  if (humidity == -1) {
    speedLimit = postedLimit;
  }

  // Now that we've finished using the speed limit and temperature, unlock the
  // mutexes so other threads can use them
  tempHumLock.unlock();
  speedLimitLock.unlock();
  // This cycle of the thread is finished, so kick the watchdog to reset the
  // timer and make sure the system continues to run without a reset
  Watchdog::get_instance().kick();
}

void getTempAndHumidity() {
  // Lock the temperature and humidity mutexes so we can safely write that data
  while (!tempHumLock.trylock()) {
  }
  errorNum = sensor.read();
  // If read() returns 0, there were no issues with the DHT 11
  // So set the temperature and humidity values to those detected by the sensor
  if (errorNum == 0) {
    temperature = sensor.getCelsius();
    humidity = sensor.getHumidity();
  } else {
    // Set the humidity to -1 to flag that there's an issue with the sensor
    humidity = -1;
  }
  // Now that we've finished with the temperature and humidity, unlock the mutex
  // so other threads can use them
  tempHumLock.unlock();
  // This cycle of the thread is finished, so kick the watchdog to reset the
  // timer and make sure the system continues to run without a reset
  Watchdog::get_instance().kick();
}

void displaySpeedLimit() {
  sign.clear();
  // Lock the speed limit mutex so we can safely read the data
  while (!speedLimitLock.trylock()) {
  }
  sign.write(speedLimit);
  // We've finished with the speed limit data, so unlock that mutex for other
  // threads to use
  speedLimitLock.unlock();
  // This cycle of the thread is finished, so kick the watchdog to reset the
  // timer and make sure the system continues to run without a reset
  Watchdog::get_instance().kick();
}

void status() {
  while (true) {

    // Since the humidity value is needed to see if there's an error, lock the
    // mutex so we can safely read the value
    while (!(tempHumLock.trylock())) {
    }
    // Humidity is set to -1 when there is some error with the DHT11
    if (humidity == -1) {
      if (errorNum == -1) {
        // Turn off the "OK" status inditcator, the green LED on PC7
        GPIOC->ODR &= ~(0x1 << 7);
        // Turn on PB7, the blue LED to signal DHTLIB_ERROR_CHECKSUM
        GPIOB->ODR |= (0x1 << 7);
      }

      else if (errorNum == -2) {
        // Turn off the "OK" status inditcator, the green LED on PC7
        GPIOC->ODR &= ~(0x1 << 7);
        // Turn on PB14, the blue LED to signal DHTLIB_ERROR_TIMEOUT
        GPIOB->ODR |= (0x1 << 14);
        // Turn off PB7, the blue LED to signal DHTLIB_ERROR_CHECKSUM
        GPIOB->ODR &= ~(0x1 << 7);
      }
    }
    // DHT 11 is OK
    else {
      // Turn on the "OK" status inditcator, the green LED on PC7
      GPIOC->ODR |= (0x1 << 7);
      // Turn off the error LEDs, PB7 and P14
      GPIOB->ODR &= ~(0x1 << 7);
      GPIOB->ODR &= ~(0x1 << 14);
    }
    // We're finished with the humidity data, so unlock the mutex to let other
    // threads use the data
    tempHumLock.unlock();
    if (abs(speedLimit - postedLimit) < 5) {
      // Flash PB8 and PB9
      // Leave the lights on for 0.5s and off for 0.5s
      GPIOB->ODR |= (0x1 << 8);
      GPIOB->ODR |= (0x1 << 9);
      ThisThread::sleep_for(500ms);
      GPIOB->ODR &= ~(0x1 << 8);
      GPIOB->ODR &= ~(0x1 << 9);
      ThisThread::sleep_for(500ms);
    }
    // This cycle of the thread is finished, so kick the watchdog to reset the
    // timer and make sure the system continues to run without a reset
    Watchdog::get_instance().kick();
  }
}

void dispatchTemp() { sensorQueue.call(getTempAndHumidity); }
void dispatchDisplay() { displayQueue.call(displaySpeedLimit); }
void dispatchSpeed() { speedQueue.call(speed); }