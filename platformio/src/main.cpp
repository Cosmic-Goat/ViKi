#include <Arduino.h>
#include <USB_Drive.h>

USB_Drive usb;

void setup()
{
	usb.begin();

	Serial.begin(115200);
	// wait for native usb
	while (!Serial)
		delay(1);

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
	// If the device is connected to usb, write uptime to a file for data visualisation
	if (tud_cdc_connected())
	{
		const uint32_t t = millis();
		usb.writeToFile("/time", (uint8_t *)&t, sizeof(t));
	}

	// write random test data to a file
	const uint32_t reading = analogRead(A1);
	usb.writeToFile("/temp.dat", (uint8_t *)&reading, sizeof(reading));

	usb.loop();
	delay(10);
}
