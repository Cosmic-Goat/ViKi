#pragma once

#include <Adafruit_SPIFlash.h>
#include <Adafruit_TinyUSB.h>
#include <SPI.h>
#include <SdFat.h>

template <typename T, size_t N> constexpr size_t array_size(T (&)[N])
{
	return N;
}

class USB_Drive
{
	static Adafruit_FlashTransport_SPI flashTransport;
	// USB Mass Storage object
	Adafruit_USBD_MSC usbMsc;

	FatFile root;
	FatFile file;

	/*
	 * Callback invoked when received READ10 command.
	 * Copy disk's data to buffer (up to bufsize) and return number of copied bytes (must be multiple of block size).
	 */
	static int32_t msc_read(uint32_t lba, void *buffer, uint32_t bufsize);

	/*
	 * Callback invoked when received WRITE10 command.
	 * Process data in buffer to disk's storage and return number of written bytes (must be multiple of block size).
	 */
	static int32_t msc_write(uint32_t lba, uint8_t *buffer, uint32_t bufsize);

	/*
	 * Callback invoked when WRITE10 command is completed (status received and accepted by host).
	 * Used to flush any pending cache.
	 */
	static void msc_flush(void);

  public:
	static Adafruit_SPIFlash flash;
	// file system object from SdFat
	static FatFileSystem fatfs;
	// Check if flash is formatted
	static bool fsFormatted;
	// Set to true when PC write to flash
	static bool fsChanged;
	void begin();
	void loop();
};
