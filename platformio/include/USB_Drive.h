#pragma once

#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <Constants.h>
#include <SPI.h>
#include <SdFat.h>

class USB_Drive
{
	// USB Mass Storage object
	Adafruit_USBD_MSC usbMsc;

  public:
	// file system object from SdFat
	static SdFat sd;
	// Set to true when PC write to flash
	static bool fsChanged;
	void begin();
	void writeToFile(const char *path, const uint8_t *buffer, const size_t size, oflag_t flags);
	void loop();
};
