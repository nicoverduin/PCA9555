/**
 * @file	clsPCA9555.cpp
 * @author 	Nico Verduin
 * @date  	dd-mm-yyyy
 *
 * @mainpage  clsPCA9555
 * Class to enable pinMode(), digitalRead() and digitalWrite() functions on IO expanders
 *
 *
 * @par Version Control
 * | Revision 	| Author 		| Date 		|
 * | :------- 	| :----- 		| :----		|
 * | $Revision$ | $Author$ 		| $Date$ 	|
 *
 *
 * @par License info
 *
 * <one line to give the program's name and a brief idea of what it does.>
 *
 * Copyright (C) 2015  Nico Verduin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Program : clsPCA9555  Copyright (C) 2015  Nico Verduin
 * This is free software, and you are welcome to redistribute it.
 *
 */

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "clsPCA9555.h"
#include "Wire.h"


/**
 * @name PCA9555 constructor
 * @param address I2C address of the IO Expander
 * Creates the class interface and sets the I2C Address of the port
 */
PCA9555::PCA9555(uint8_t address) {
	_address = address;		// save the address id
	Wire.begin();			// start I2C communication
}

/**
 * @name pinMode
 * @param pin		pin number
 * @param IOMode	mode of pin INPUT or OUTPUT
 * sets the mode of this IO pin
 */
void PCA9555::pinMode(uint8_t pin, uint8_t IOMode) {
	//
	// check valid pin first
	//
	if (pin <= 15) {
		//
		// now set the correct bit in the configuration register
		//
		if (IOMode == OUTPUT) {
			//
			// mask correct bit to 0 by inverting x so that only
			// the correct bit is LOW. The rest stays HIGH
			//
;
			_configurationRegister = _configurationRegister && ~(1 << pin);
		} else {
			//
			// or just the required bit to 1
			//
			_configurationRegister = _configurationRegister || (1 << pin);
		}
		//
		// write configuration register to chip
		//
		Wire.beginTransmission(_address);  			// setup direction register
		Wire.write(0x06);  							// Port A
		Wire.write(_configurationRegister_low);
		_error = Wire.endTransmission();

#ifdef DEBUG
Serial.print("pinMode : Configure Port A _error = ");
Serial.println(_error);
#endif
		Wire.beginTransmission(_address);  			// setup direction register
		Wire.write(0x07);  							// port B
		Wire.write(_configurationRegister_high);
		_error = Wire.endTransmission();

#ifdef DEBUG
Serial.print("pinMode : Configure Port B _error = ");
Serial.println(_error);
#endif
	}
}
/**
 * @name digitalRead Reads the high/low value of specified pin
 * @param pin
 * @return value of pin
 */
uint8_t PCA9555::digitalRead(uint8_t pin) {
	//
	// we wil only process pins <= 15
	//
	if (pin <= 15) {
		//
		// read the address input register
		//
		Wire.beginTransmission(_address);  			// setup read registers
		Wire.requestFrom(NXP_INPUT, 2);  			// pointer to input register address 0
		_error = Wire.endTransmission();

#ifdef DEBUG
Serial.print("digitalRead : Read Input register error code = ");
Serial.println(_error);
#endif

		//
		// wait for data
		//
		while (Wire.available() != 2 ) {};
		//
		// read data
		//
		_valueRegister_low 	= Wire.read();  		// Input register 0
		_valueRegister_high = Wire.read();  		// Input register 1

#ifdef DEBUG
Serial.print("digitalRead : value of INPUT register reads = ");
Serial.println(_valueRegister, BIN);
#endif

		//
		// now mask the bit required and see if it is a HIGH
		//
		if ((_valueRegister & (1 << pin)) > 0){
			//
			// the bit is HIGH otherwise we would hava a zero value
			//
			return HIGH;
		} else {
			return LOW;
		}
	} else {
		//
		// invalid pin
		//
		return 255;
	}
}

void PCA9555::digitalWrite(uint8_t pin, uint8_t value) {
	uint16_t valueToSend;
	//
	// check valid pin first
	//
	if (pin <= 15) {
		//
		// next convert the value we want to set to HIGH or LOW
		//
		if (value > 0){
			valueToSend = HIGH;
		} else {
			valueToSend = LOW;
		}
		//
		// now read the output register first.
		//
		Wire.beginTransmission(_address);  			// setup read registers
		Wire.requestFrom(NXP_OUTPUT, 2);  			// pointer to input register address 0
		_error = Wire.endTransmission();

#ifdef DEBUG
Serial.print("digitalWrite : Read OUTPUT register Error code = ");
Serial.println(_error);
#endif

		//
		// wait for data
		//
		while (Wire.available() != 2 ) {};
		//
		// and read the actual data
		//
		_valueRegister_low 	= Wire.read();  		// Output register 0
		_valueRegister_high = Wire.read();  		// Output register 1


#ifdef DEBUG
Serial.print("digitalRead : value of OUTPUT register reads = ");
Serial.println(_valueRegister, BIN);
#endif

		//
		// next set the bit we want to set
		// if the value is LOW we will and the register value with correct bit set to zero
		// if the value is HIGH we will or the register value with correct bit set to HIGH
		//
		if (valueToSend) {
			//
			// this is a High value so we will or it with the value register
			//
			_valueRegister = _valueRegister | (valueToSend << pin);	// and OR bit in register
		} else {
			//
			// this is a LOW value so we have to AND it with 0 into the _valueRegister
			//
			_valueRegister = _valueRegister & ~(valueToSend << pin);	// AND all bits
		}
		//
		// write output register to chip
		//
		Wire.beginTransmission(_address);  			// setup direction registers
		Wire.write(0x06);  							// pointer to DDR address 0
		Wire.write(_configurationRegister_low);  	// DDR Port0
		_error = Wire.endTransmission();

#ifdef DEBUG
Serial.print("digitalWrite : Write Output register 0 error code = ");
Serial.println(_error);
#endif

		Wire.beginTransmission(_address);  			// setup direction registers
		Wire.write(0x07);  							// pointer to DDR address 0
		Wire.write(_configurationRegister_high);  	// DDR Port1
		_error = Wire.endTransmission();

#ifdef DEBUG
Serial.print("digitalWrite : Write Output register 1 error code = ");
Serial.println(_error);
#endif

	}
}

