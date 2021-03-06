// Code Adapted from DHT11 Library by Eric Fossum from the MBED Code Repository
// For use in CSE321 at UB 
// Copyright 2016 Eric Fossum
// SPDX-License-Identifier: Apache-2.0
// https://os.mbed.com/users/fossum_13/code/DHT11/
// Licensed with Apache http://www.apache.org/licenses/ 
// Depreciation issues addressed 11/26/2020


#include "DHT.h"
 
DHT11::DHT11(PinName const &p) : _pin(p) {
    // Set creation time so we can make 
    // sure we pause at least 1 second for 
    // startup.
    _timer.start();
    _temperature = 0; //default unit of Celcius 
    _humidity = 0;
}

int DHT11::read() {
    // can not read more frequent than every 2 seconds
  
    // BUFFER TO RECEIVE
    uint8_t bits[5]; // DHT11 is a 40 bit signal, grouped in 5 bytes, each byte has own purpose
    uint8_t cnt = 7; //byte bit tracker
    uint8_t idx = 0; // bit set tracking
    //read in MSB to LSB 
 
    // EMPTY BUFFER
    for (int i=0; i< 5; i++) bits[i] = 0;
    
    // Verify sensor settled after boot
    while(_timer.elapsed_time().count() < 1500) {}
    _timer.stop();
 
    // Notify it we are ready to read
    _pin.output();
    _pin = 0;
    thread_sleep_for(18);
    _pin = 1;
    wait_us(40);
    _pin.input();
 
    // ACKNOWLEDGE or TIMEOUT
    unsigned int loopCnt = 10000;
    while(_pin == 0)
        if (loopCnt-- == 0) return DHTLIB_ERROR_TIMEOUT;
 
    loopCnt = 10000;
    while(_pin == 1)
        if (loopCnt-- == 0) return DHTLIB_ERROR_TIMEOUT;
 
    // READ OUTPUT - 40 BITS => 5 BYTES or TIMEOUT
    for (int i=0; i<40; i++)
    {
        loopCnt = 10000;
        while(_pin == 0)
            if (loopCnt-- == 0) return DHTLIB_ERROR_TIMEOUT;
 
        //unsigned long t = micros();
        Timer t;
        t. start();
 
        loopCnt = 10000;
        while(_pin == 1) //track how long value is 1
            if (loopCnt-- == 0) return DHTLIB_ERROR_TIMEOUT;

        //26-30us is 0, ~70us is 1, 40 is a good sample point
        if (t.elapsed_time().count() > 40) bits[idx] |= (1 << cnt);
        if (cnt == 0)   // next byte?
        {
            cnt = 7;    // restart at MSB
            idx++;      // next byte!
        }
        else cnt--;
    }
 
    // WRITE TO RIGHT VARS
    // as bits[1] and bits[3] are allways zero they are omitted in formulas.
    _humidity    = bits[0];
    _temperature = bits[2];
    uint8_t sum = bits[0] + bits[2];  
 
    if (bits[4] != sum) return DHTLIB_ERROR_CHECKSUM;
    return DHTLIB_OK;
}
 
float DHT11::getFahrenheit() { //performs C to F conversion
    return((_temperature * 1.8) + 32);
}
 
int DHT11::getCelsius() {
    return(_temperature);
}
int DHT11::getHumidity() {
    return(_humidity);
}
