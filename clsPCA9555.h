/*
 * clsPCA9555.h
 *
 *  Created on: 27 jul. 2015
 *      Author: Nico
 */

#ifndef CLSPCA9555_H_
#define CLSPCA9555_H_

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define DEBUG 1

/** enum with names of ports ED0 - ED15 */
enum {
    ED0, ED1, ED2 , ED3 , ED4 , ED5 , ED6 , ED7 ,
    ED8, ED9, ED10, ED11, ED12, ED13, ED14, ED15
};

//
// PCA9555 defines
//
#define NXP_INPUT      0
#define NXP_OUTPUT     2
#define NXP_INVERT     4
#define NXP_CONFIG     6

class PCA9555 {
public:
    PCA9555(uint8_t address);                            // constructor
    void pinMode(uint8_t pin, uint8_t IOMode );          // pinMode
    uint8_t digitalRead(uint8_t pin);                    // digitalRead
    void digitalWrite(uint8_t pin, uint8_t value );      // digitalWrite

private:
    //
    // low level methods
    //
    uint16_t I2CGetValue(uint8_t address, uint8_t reg);
    void I2CSetValue(uint8_t address, uint8_t reg, uint8_t value);

    union {
        struct {
            uint8_t _configurationRegister_low;          // low order byte
            uint8_t _configurationRegister_high;         // high order byte
        };
        uint16_t _configurationRegister;                 // 16 bits presentation
    };
    union {
        struct {
            uint8_t _valueRegister_low;                  // low order byte
            uint8_t _valueRegister_high;                 // high order byte
        };
        uint16_t _valueRegister;
    };
    uint8_t _address;                                    // address of port this class is supporting
    int _error;                                          // error code from I2C
};


#endif /* CLSPCA9555_H_ */
