# IoAbstraction for Arduino and mbed summary

Dave Cherry / TheCodersCorner.com make this library available for you to use. It takes me significant effort to keep all my libraries current and working on a wide range of boards. Please consider making at least a one off donation via the sponsor button if you find it useful. In forks, please keep text to here intact.

This library provides several useful extensions that make programming Arduino / mbed for non-trivial apps simpler. There are many different practical and familiar examples packaged with it in the `examples` folder. Below I cover each of the main functions briefly with a link to more detailed documentation. The API is almost identical between Arduino and mbed making it easier to port between the two. 

## Documentation and questions

Along with ths quick start guide and the examples also see:

* [IoAbstraction documentation pages](https://www.thecoderscorner.com/products/arduino-libraries/io-abstraction/)
* [IoAbstraction reference documentation](https://www.thecoderscorner.com/ref-docs/ioabstraction/html)

There is a forum where questions can be asked, but the rules of engagement are: **this is my hobby, I make it available because it helps others**. Don't expect immediate answers, make sure you've recreated the problem in a simple sketch that you can send to me. Please consider making at least a one time donation using the sponsor link above before using the forum.

* [TCC Libraries community discussion forum](https://www.thecoderscorner.com/jforum/)
* I also monitor the Arduino forum [https://forum.arduino.cc/], Arduino related questions can be asked there too.

## Installation for Arduino IDE

This library is available in library manager on both Arduino and PlatformIO, this is the best choice for most people. It should automatically install the dependencies, [TaskManagerIO](https://github.com/davetcc/TaskManagerIO) and [SimpleCollections](https://github.com/davetcc/SimpleCollections). If for some reason it does not install the dependency, please also install it manually. It is highly recommended that you install the libraries using your library manager.

## Installation for PlatformIO (Arduino or mbed)

Use the platformIO library manager to get the library. It's called 'IoAbstraction'. It should automatically include "TaskManagerIO" and "SimpleCollections" as it's a dependency.

## This library is based on TaskManagerIO and SimpleCollections

Take a look at the [TaskManagerIO repo](https://github.com/davetcc/TaskManagerIO) for more information about how task manager works, this library relies heavily on task manager.

Also, this library uses [SimpleCollections](https://github.com/davetcc/SimpleCollections) within switches and a few other areas.

## BasicIoAbstraction - easily interchange between pins, PCF8574, MCP23017 and shift registers.

Lets you choose to use Arduino pins, shift register Input/Output, PCF8574 i2c and MCP23017 i2c in an inter-changable way. Use it in your sketch to treat shift registers or i2c expanders like pins. There's even an abstraction that can combine together Arduino pins and one or more other expander! See the documentation (link further up) for more details.

If you are building a library and want it to work with either Arduino pins, shift registers or an IO expander for IO, then this library is probably a good starting point.

A simple example:

If we want to use the i2c wire based ioFrom8574 we must include the wire header file

	#include <IoAbstractionWire.h>

At the global level (outside of any function) we create an i2c expander on address 0x20:

	IoAbstractionRef ioExpander = ioFrom8574(0x20);

Or for Arduino pins instead..
	
	IoAbstractionRef ioUsingArduino();

And lastly for DfRobot LCD shield input we use (requires library V1.3.2 at least):

	IoAbstractionRef inputFromDfRobotShield();   // for all other versions
	IoAbstractionRef inputFromDfRobotShieldV1(); // for version 1

In setup we set it's first IO pin to input and start the Wire library:
	
	Wire.begin();  
 	ioDevicePinMode(ioExpander, 0, INPUT);
  
And then later we read from it, in this case as we are doing a single read, use the 'S' version of the method as it removes the need to call the sync method. The only limitation is we must synchronize the device state. This allows us to be efficient where possible, setting several pins, syncing and then reading pins.

  	int valueRead = ioDeviceDigitalReadS(ioExpander, 0); // read pin 0 on ioExpander

Let's now say we wanted to write one value and read two items on the same device, in this case we don't use the 'S' version of the method, because otherwise it would sync three times.

	ioDeviceDigitalWrite(ioExpander, outputPin, HIGH);
	ioDeviceSync(ioExpander);
	int read1 = ioDeviceDigitalRead(ioExpander, inputPin1);
	int read2 = ioDeviceDigitalRead(ioExpander, inputPin2);

## SwitchInput - buttons that are debounced with event based callbacks

This class provides an event based approach to handling switches and rotary encoders. It full debounces switches before calling back your event handler and handles both repeat key and held down states. In the case of rotary encoders an interrupt on PIN_A is required, as the library needs to react very quickly; it is also important to make sure you have no long running tasks, or you'll miss the delayed rise. Note that this component also uses task manager.

Before doing anything else, you must add taskManager's run loop to your loop method, and your loop method must not do any long delay calls.

	void loop() {
		taskManager.runLoop();
	}

Here's a simple example using a switch:

In setup we initialise it telling it to use arduino pins for IO, we could use shift registers or an i2c expander, and we also add a switch along with the event that should be:

	switches.initialise(ioUsingArduino(), pullUpLogic); // pull up logic is optional, defaults to PULL_DOWN buttons.
	switches.addSwitch(spinwheelClickPin, onClicked, NO_REPEAT); // NO_REPEAT is optional, sets the repeat interval in 100s of second.

Then we create a function for onClicked, this will be called when the button is pressed:

	void onClicked(uint8_t pin, bool heldDown) {
		// pin: the pin that was pressed
    		// heldDown: if the button has been held down
  	}

It is also possible to use initialiseInterrupt instead of initialise, when using this mode the library does not poll the switches unless a button is pressed down. It's use
is interchangable with initialise().

## RotaryEncoder - hardware and button emulation, even available with i2c IO expanders

Switch input also fully supports rotary encoders (and simulated rotary encoders using up / down buttons). For this you just initialise the rotary
encoder, but note that for rotary encoders PIN_A must be an interrupt pin, such as pin 2 on most boards. No debouncing is needed, the library
will switch on pull up resistors too, but you may need lower resistance pull ups will long wire runs.

For more see https://www.thecoderscorner.com/products/arduino-libraries/io-abstraction/arduino-switches-handled-as-events/.

First we must register the callback function that will be called when there's a change

	void onEncoderChange(int newValue) {
		// do something with new value..
	}

Then we create an encoder using one of the three examples below

	// Example 1, Real encoder, we need to set up the pins that the encoder uses and provide a callback
	setupRotaryEncoderWithInterrupt(encoderAPin, encoderBPin, onEncoderChange);
	
	// Example 2, Up / down buttons acting like an encoder
	setupUpDownButtonEncoder(pinUpBtn, pinDownBtn, onEncoderChange);

	// Example 3, advanced usage, same as example 1, but with two encoders
	HardwareRotaryEncoder* firstEncoder = new HardwareRotaryEncoder(firstEncoderAPin, firstEncoderBPin, onFirstEncoderChange);
	HardwareRotaryEncoder* secondEncoder = new HardwareRotaryEncoder(secondEncoderAPin, secondEncoderBPin, onSecondEncoderChange);
	switches.setEncoder(0, firstEncoder);
	switches.setEncoder(1, secondEncoder);

For the vast majority of encoders there is no need to provide the encoder type. If you have a quarter cycle rotary encoder, there is an extra optional constructor parameter for the encoder type, thanks go to @ddd999 for this support. The options are listed below: 

    /** Detent after every signal change, A or B */
    QUARTER_CYCLE,
    /** Detent on every position where A == B */
    HALF_CYCLE,
    /** Detent after every full cycle of both signals, A and B */ 
    FULL_CYCLE

Then lastly we set the precision of the encoder (IE the range), if the current and maximum value are both 1, then the mode is direction only.

	// After initialising, we set the maximum value (from 0) that the encoder represents
	// along with the current value
	switches.changeEncoderPrecision(maximumEncoderValue, 100);

	// advanced usage: if you want to change the precision of other than the first encoder
	switches.changeEncoderPrecision(1, maximumValue, currentValue);

### Notes for using more than rotary encoder at the same time

There are a few limitations with multiple encoders. Firstly, the encoders must all be on the same input device, such that the interrupt
comes from a device that is shared by them all. For example the they should all share an IO device such as a 23017 or an 8574, or if on 
arduino pins, all the A pins must be interrupt driven. Secondly, there is a hard limit on the number defined by `MAX_ROTARY_ENCODERS` which 
you can change by altering the file `SwitchInput.h` should you need more (or less) than 4.

## EepromAbstraction - support for both AVR and i2c AT24 EEPROMs with a common interface

The eeprom abstraction has several implementations, which makes it possible for libraries and code to be transparent from
AVR or AT24 based I2C eeprom storage, it even has a No-Op implementation as well. All the implementations shown below are interchangable
so if like me you switch between 8 and 32 bit boards, just change the EEPROM implementation!

### Avr Eeprom abstraction

This implementation uses the standard AVR EEPROM space for storage - only available on 8bit AVR such as Uno, MEGA.

To create an instance

	AvrEeprom avrEeprom;

### I2c AT24 EEPROM abstraction

A ground up implementation of the i2c eeprom protocol that should be compatible with the vast majority of i2c EEPROM devices.

To create an instance we pass the address of the chip (usually between 0x50 and 0x57), and also the page size, below are the page sizes 
for the most common devices. Consult the datasheet if unsure.

| ROM       | PageSize |
|-----------|----------|
| AT24C32   |       32 |
| AT24C64   |       32 |
| AT24C128  |       64 |
| AT24C256  |       64 |
| AT24C512  |      128 |


	I2cAt24Eeprom anEeprom(addressOfRom, pageSize);
 
Then during setup, you must ensure you call Wire.begin()

	void setup() {
		Wire.begin();
		
		// your other setup code.
	}
 
 
 ### NoEeprom - does nothing, but fulfills the interface.

Does nothing but implements the interface - useful sometimes..

To create an instance

	NoEeprom anEeprom;

 
### Reading and writing EEPROM values

Writing primitive values
 
	anEeprom.write8(romAddr, byteVal);
	anEeprom.write16(romAddr, value16);
	anEeprom.write32(romAddr, value32);

	byte by = anEeprom.read8(romStart);
	unsigned int i16 = anEeprom.read16(romStart);
	unsigned long i32 = anEeprom.read32(romStart);
	
Writing arrays and strings

	char data[20]; // example array to work with
	anEeprom.readIntoMemArray((unsigned char*)data, romStart, sizeof data);
	anEeprom.writeArrayToRom(romStart, (const unsigned char*)data, sizeof data);

### Analog device abstraction

Since 1.4 a new abstraction for analog devices has been added, it allows for an interchangable interface between most analog read and write devices such as ADC, DAC, PWM, Volume controls and Digital Potentiometers. At the moment the only available one is the Arduino pin based implementation. See the `analogExample` for usage.

Note that although the Arduino constructor allows the bit depth to be set, it only has any effect on SAMD boards. 

```
    // create the analog device
    ArduinoAnalogDevice analog;

    // to make A1 an input
    analog.initPin(A1, DIR_IN);
    
    // and make the PWM_PIN output.
    analog.initPin(PWM_PIN, DIR_OUT);

    // returns the range of the pin requested in the direction specified.
	int range = getMaximumRange(DIR_IN, A1);

    // to read from A1
    int reading = analog.getCurrentValue(A1);

    // to write to PWM_PIN
    analog.setCurrentValue(PWM_PIN, newValue);
```

## ESP32 extras mode

On ESP32 we are slowly adding support for direct IDF, as it's been requested by one of our clients. This will slowly appear over several releases. To enable this mode you can set the flag `IOA_USE_ESP32_EXTRAS`. Once you do this IDF functions are used for all digital IO functions. We always use IDF functions for analog input and DAC output, and are slowly moving toward direct LTDC functions for PWM instead of wrappers. 

## Other links

[https://www.thecoderscorner.com/electronics/microcontrollers/switches-inputs/basic-io-abstraction-library-pins-or-8574/]

## Making changes to IoAbstraction

We welcome people rolling up their sleeves and helping out, but please do reach out to us before starting any work, so we can ensure it's in sync with our development. We use platformIO for development and have a specific project available to help you get started, along with tests that check many elements still work as expected. See [https://github.com/davetcc/tcLibraryDev]
