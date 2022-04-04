#include <Adafruit_SPIFlash.h>
#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>

Adafruit_FlashTransport_SPI flashTransport(EXTERNAL_FLASH_USE_CS, EXTERNAL_FLASH_USE_SPI);
Adafruit_SPIFlash flash(&flashTransport);

// Create an array of data structures and fill it with correct flash settings
static const SPIFlash_Device_t my_flash_devices[] = {W25Q256JV};
// Specify that only a single flash chip is listed in the array we just created.
const int flashDevices = 1;

// USB Mass Storage object
Adafruit_USBD_MSC usb_msc;

// file system object from SdFat
FatFileSystem fatfs;
FatFile root;
FatFile file;
// Check if flash is formatted
bool fs_formatted;
// Set to true when PC write to flash
bool fs_changed;

/*
 * Callback invoked when received READ10 command.
 * Copy disk's data to buffer (up to bufsize) and return number of copied bytes (must be multiple of block size).
 */
int32_t msc_read(uint32_t lba, void *buffer, uint32_t bufsize)
{
	// Note: SPIFLash Block API: readBlocks/writeBlocks/syncBlocks already include 4K sector caching internally.
	return flash.readBlocks(lba, (uint8_t *)buffer, bufsize / 512) ? bufsize : -1;
}

/*
 * Callback invoked when received WRITE10 command.
 * Process data in buffer to disk's storage and return number of written bytes (must be multiple of block size).
 */
int32_t msc_write(uint32_t lba, uint8_t *buffer, uint32_t bufsize)
{
	// Note: SPIFLash Block API: readBlocks/writeBlocks/syncBlocks already include 4K sector caching internally.
	return flash.writeBlocks(lba, buffer, bufsize / 512) ? bufsize : -1;
}

/*
 * Callback invoked when WRITE10 command is completed (status received and accepted by host).
 * Used to flush any pending cache.
 */
void msc_flush(void)
{
	// sync with flash
	flash.syncBlocks();
	// clear file system's cache to force refresh
	fatfs.cacheClear();
	fs_changed = true;
}

void setup()
{
	// Give the flash driver detals about our flash chip
	flash.begin(my_flash_devices, flashDevices);
	// Set disk vendor id, product id and revision with string up to 8, 16, 4 characters respectively
	usb_msc.setID("Team 1", "VitalKit", "0.1");
	// Set callback
	usb_msc.setReadWriteCallback(msc_read, msc_write, msc_flush);
	// Set disk size, block size should be 512 regardless of spi flash page size
	usb_msc.setCapacity(flash.size() / 512, 512);
	// MSC is ready for read/write
	usb_msc.setUnitReady(true);
	usb_msc.begin();

	// Init file system on the flash
	fs_formatted = fatfs.begin(&flash);

	Serial.begin(115200);
	// while ( !Serial ) delay(10);   // wait for native usb

	if (!fs_formatted)
	{
		Serial.println("Failed to init files system, flash may not be formatted");
	}

	Serial.println("Adafruit TinyUSB Mass Storage External Flash example");
	Serial.print("JEDEC ID: 0x");
	Serial.println(flash.getJEDECID(), HEX);
	Serial.print("Flash size: ");
	Serial.print(flash.size() / 1024);
	Serial.println(" KB");

	fs_changed = true; // to print contents initially
}

void loop()
{
	if (fs_changed)
	{
		fs_changed = false;

		// check if host formatted disk
		if (!fs_formatted)
		{
			fs_formatted = fatfs.begin(&flash);
		}

		// skip if still not formatted
		if (!fs_formatted)
			return;

		Serial.println("Opening root");

		if (!root.open("/"))
		{
			Serial.println("open root failed");
			return;
		}

		Serial.println("Flash contents:");

		// Open next file in root.
		// Warning, openNext starts at the current directory position
		// so a rewind of the directory may be required.
		while (file.openNext(&root, O_RDONLY))
		{
			file.printFileSize(&Serial);
			Serial.write(' ');
			file.printName(&Serial);
			if (file.isDir())
			{
				// Indicate a directory.
				Serial.write('/');
			}
			Serial.println();
			file.close();
		}

		root.close();

		Serial.println();
		delay(1000); // refresh every 1 second
	}
}

