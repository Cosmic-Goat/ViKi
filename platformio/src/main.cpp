#include <Adafruit_MCP9808.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <USB_Drive.h>
#include <Wire.h>

USB_Drive usb;
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
Adafruit_NeoPixel pixels(1, PIN_NEOPIXEL);

union {
	float fl;
	uint32_t uint;
	uint8_t bytes[4];
} data;

void setup()
{
	Serial.begin(9800);
	// while (!Serial)
	// 	delay(1); // Wait for serial monitor to open.

	// Set onboard pixel to a nice yellow, so we can tell the MCU is on and running our code.
	pixels.begin();
	pixels.setBrightness(128);
	pixels.setPixelColor(0, pixels.Color(255, 128, 0));
	pixels.show();

	usb.begin();

	usb.fsChanged = true; // to print contents initially

	if (!tempsensor.begin(0x18))
	{
		Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct.");
	}

	tempsensor.setResolution(3); // sets the resolution mode of reading, the modes are defined in the table bellow:
								 // Mode Resolution SampleTime
								 //  0    0.5°C       30 ms
								 //  1    0.25°C      65 ms
								 //  2    0.125°C     130 ms
								 //  3    0.0625°C    250 ms
}

void loop()
{
	Serial.println("wake up MCP9808.... "); // wake up MCP9808 - power consumption ~200 mikro Ampere
	tempsensor.wake();						// wake up, ready to read!

	// Read and print out the temperature, also shows the resolution mode used for reading.
	Serial.print("Resolution in mode: ");
	Serial.println(tempsensor.getResolution());
	float c = tempsensor.readTempC();
	float f = tempsensor.readTempF();
	Serial.print("Temp: ");
	Serial.print(c, 4);
	Serial.print("*C\t and ");
	Serial.print(f, 4);
	Serial.println("*F.");

	delay(2000);
	Serial.println("Shutdown MCP9808.... ");
	tempsensor.shutdown_wake(1); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere, stops temperature sampling
	Serial.println("");
	delay(200);

	// If the device is connected to usb, write uptime to a file for data visualisation
	if (tud_cdc_connected())
	{
		data.uint = millis();
		usb.writeToFile("/time", data.bytes, sizeof(data.bytes));
	}

	// write random test data to a file
	usb.writeToFile("/temp.dat", (uint8_t *)&c, sizeof(c));

	usb.loop();
	delay(10);
}
