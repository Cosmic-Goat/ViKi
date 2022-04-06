#pragma once

#include <Adafruit_SPIFlash.h>
#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>

class USB_Drive
{
	static Adafruit_FlashTransport_SPI flashTransport;
	// USB Mass Storage object
	Adafruit_USBD_MSC usbMsc;

  public:
	static Adafruit_SPIFlash flash;
	// file system object from SdFat
	static FatFileSystem fatfs;
	// Check if flash is formatted
	static bool fsFormatted;
	// Set to true when PC write to flash
	static bool fsChanged;
	void begin();
	void writeToFile(const String &path, const uint8_t *buffer, const size_t size);
	void loop();
};
