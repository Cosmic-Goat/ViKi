// Defines required for SAMDTimerInterrupt library
#define TIMER_INTERRUPT_DEBUG 0
#define _TIMERINTERRUPT_LOGLEVEL_ 0

#include <Adafruit_MCP9808.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <Constants.h>
#include <SAMDTimerInterrupt.h>
#include <USB_Drive.h>
#include <Wire.h>

USB_Drive usb;
SAMDTimer iTimer(TIMER_TC3);
Adafruit_MCP9808 tempSensor;
Adafruit_NeoPixel pixels(1, PIN_NEOPIXEL);

volatile bool tempPoll = true;
bool usbConnected = false;

// union {
// 	float fl;
// 	uint32_t uint;
// 	uint8_t bytes[4];
// } data;

void tempTimer()
{
	tempPoll = true;
}

void logTime(const char *path)
{
	const uint32_t time = millis();
	usb.writeToFile(path, (uint8_t *)&time, sizeof(time), O_WRITE | O_CREAT);
	usbConnected = true;
}

void setup()
{
	Serial.begin(9800);

	// if (printDebug)
	// {
	// 	// Wait for serial monitor to open.
	// 	while (!Serial)
	// 		delay(1);
	// }

	// Set onboard pixel to a nice yellow, so we can tell the MCU is on and running our code.
	pixels.begin();
	pixels.setBrightness(128);
	pixels.setPixelColor(0, pixels.Color(255, 128, 0));
	pixels.show();

	usb.begin();
	tempSensor.begin(0x18);

	// if (!)
	// {
	// 	Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct.");
	// }

	// sets the resolution mode of reading, the modes are defined in the table bellow:
	// Mode Resolution SampleTime
	//  0    0.5°C       30 ms
	//  1    0.25°C      65 ms
	//  2    0.125°C     130 ms
	//  3    0.0625°C    250 ms
	tempSensor.setResolution(3);

	// Interval in microsecs
	if (iTimer.attachInterruptInterval(tempInterval, tempTimer))
	{
		Serial.print(F("Starting ITimer OK, millis() = "));
		Serial.println(millis());
	}
	else
		Serial.println(F("Can't set ITimer. Select another freq. or timer"));

	logTime("/sTime");
}

void loop()
{
	// noInterrupts();
	if (tempPoll)
	{
		// Wake up MCP9808, power consumption ~0.2 mA
		tempSensor.wake();
		const float temp = tempSensor.readTempC();
		tempSensor.shutdown();
		usb.writeToFile("/temp.dat", (uint8_t *)&temp, sizeof(temp), O_WRITE | O_CREAT | O_APPEND);
	}
	// interrupts();

	// If the device is connected to usb, write uptime to a file for data visualisation
	if (tud_cdc_connected() && !usbConnected)
	{
		logTime("/endTime");
		usbConnected = true;
	}

	usb.loop();
}
