#include <Adafruit_MCP9808.h>
#include <Arduino.h>
#include <USB_Drive.h>
#include <Wire.h>

USB_Drive usb;
// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

union {
	float fl;
	uint32_t uint;
	uint8_t bytes[4];
} data;

void setup()
{
	usb.begin();

	Serial.begin(115200);

	if (!usb.fsFormatted)
	{
		Serial.println("Failed to init files system, flash may not be formatted");
	}

	Serial.println("Adafruit TinyUSB Mass Storage External Flash example");
	Serial.print("Flash size: ");
	Serial.print(usb.flash.size() / 1024);
	Serial.println(" KB");

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
		usb.writeToFile("time", data.bytes, sizeof(data.bytes));
	}

	// write random test data to a file
	usb.writeToFile("temp.dat", (uint8_t *)&c, sizeof(c));

	usb.loop();
	delay(10);
}
