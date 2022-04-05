#include <Adafruit_SPIFlash.h>
#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>
#include <USB_Drive.h>

USB_Drive usb;

void setup()
{
	usb.begin();

	Serial.begin(115200);
	// while ( !Serial ) delay(10);   // wait for native usb

	if (!usb.fsFormatted)
	{
		Serial.println("Failed to init files system, flash may not be formatted");
	}

	Serial.println("Adafruit TinyUSB Mass Storage External Flash example");
	Serial.print("JEDEC ID: 0x");
	Serial.println(usb.flash.getJEDECID(), HEX);
	Serial.print("Flash size: ");
	Serial.print(usb.flash.size() / 1024);
	Serial.println(" KB");

	usb.fsChanged = true; // to print contents initially
}

void loop()
{
	usb.loop();
	delay(1000); // refresh every 1 second
}
