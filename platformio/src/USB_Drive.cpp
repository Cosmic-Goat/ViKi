#include <USB_Drive.h>

// Create an array of data structures and fill it with correct flash settings
constexpr static SPIFlash_Device_t flashDevices[] = {W25Q256JV};
Adafruit_FlashTransport_SPI USB_Drive::flashTransport(EXTERNAL_FLASH_USE_CS, EXTERNAL_FLASH_USE_SPI);
Adafruit_SPIFlash USB_Drive::flash(&flashTransport);

FatFileSystem USB_Drive::fatfs;
bool USB_Drive::fsFormatted = false;
bool USB_Drive::fsChanged = false;

int32_t USB_Drive::msc_read(uint32_t lba, void *buffer, uint32_t bufsize)
{
	// Note: SPIFLash Block API: readBlocks already include 4K sector caching internally.
	return flash.readBlocks(lba, (uint8_t *)buffer, bufsize / 512) ? bufsize : -1;
}

int32_t USB_Drive::msc_write(uint32_t lba, uint8_t *buffer, uint32_t bufsize)
{
	// Note: SPIFLash Block API: writeBlocks already include 4K sector caching internally.
	return flash.writeBlocks(lba, buffer, bufsize / 512) ? bufsize : -1;
}

void USB_Drive::msc_flush(void)
{
	// sync with flash
	flash.syncBlocks();
	// clear file system's cache to force refresh
	fatfs.cacheClear();
	fsChanged = true;
}

void USB_Drive::begin()
{
	// Give the flash driver detals about our flash chip
	flash.begin(flashDevices, array_size(flashDevices));
	// Set disk vendor id, product id and revision with string up to 8, 16, 4 characters respectively
	usbMsc.setID("Team 1", "VitalKit", "0.1");
	// Set callback
	usbMsc.setReadWriteCallback(msc_read, msc_write, msc_flush);
	// Set disk size, block size should be 512 regardless of spi flash page size
	usbMsc.setCapacity(flash.size() / 512, 512);
	// MSC is ready for read/write
	usbMsc.setUnitReady(true);
	usbMsc.begin();

	// Init file system on the flash
	fsFormatted = fatfs.begin(&flash);
}

void USB_Drive::loop()
{
	if (fsChanged)
	{
		fsChanged = false;

		// check if host formatted disk
		if (!fsFormatted)
		{
			fsFormatted = fatfs.begin(&flash);
		}

		// skip if still not formatted
		if (!fsFormatted)
			return;

		Serial.println("Opening root");

		if (!root.open("/"))
		{
			Serial.println("open root failed");
			return;
		}

		Serial.println("Flash contents:");

		// Open next file in root.
		// Warning, openNext starts at the current directory position so a rewind of the directory may be required.
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
	}
}
