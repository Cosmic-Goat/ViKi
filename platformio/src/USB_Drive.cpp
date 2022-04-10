#include <USB_Drive.h>

/*
 * constexpr function to get size of static array at compile time.
 */
template <typename T, size_t N> constexpr size_t array_size(T (&)[N])
{
	return N;
}

// SD Card Chip Select Pin
constexpr uint32_t SDCARD_CS = 7;
SdFat USB_Drive::sd;
bool USB_Drive::fsChanged = false;

void USB_Drive::begin()
{
	Serial.println("INIT SD");
	// Set disk vendor id, product id and revision with string up to 8, 16, 4 characters respectively
	usbMsc.setID("Team 1", "VitalKit", "0.1");
	// Initializing the SD card can take time.
	// If it takes too long, our board could be enumerated as a CDC device only, i.e without Mass Storage.
	// To prevent this, we call Mass Storage begin first and LUN readiness will be set later on.
	usbMsc.begin();

	if (!sd.begin(SDCARD_CS, SD_SCK_MHZ(50)))
	{
		Serial.println("Failed ");
		return;
	}

	/*
	 * Callback invoked when received READ10 command.
	 * Copy disk's data to buffer (up to bufsize) and return number of copied bytes (must be multiple of block size).
	 */
	const auto msc_read = [](uint32_t lba, void *buffer, uint32_t bufsize) {
		// SPIFlash Block API includes 4K sector caching internally.
		const bool read = sd.card()->readBlocks(lba, (uint8_t *)buffer, bufsize / 512);
		return read ? (int32_t)bufsize : -1;
	};

	/*
	 * Callback invoked when received WRITE10 command.
	 * Process data in buffer to disk's storage and return number of written bytes (must be multiple of block size).
	 */
	const auto msc_write = [](uint32_t lba, uint8_t *buffer, uint32_t bufsize) {
		// SPIFlash Block API includes 4K sector caching internally.
		const bool write = sd.card()->writeBlocks(lba, buffer, bufsize / 512) ? bufsize : -1;
		return write ? (int32_t)bufsize : -1;
	};

	/*
	 * Callback invoked when WRITE10 command is completed (status received and accepted by host).
	 * Used to flush any pending cache.
	 */
	const auto msc_flush = [](void) {
		// sync with flash
		sd.card()->syncBlocks();
		// clear file system's cache to force refresh
		sd.cacheClear();
		fsChanged = true;
	};

	usbMsc.setReadWriteCallback(msc_read, msc_write, msc_flush);

	const uint32_t block_count = sd.card()->cardSize();
	usbMsc.setCapacity(block_count, 512);

	fsChanged = true; // to print contents initially

	Serial.print("OK, Card size = ");
	Serial.print((block_count / (1024 * 1024)) * 512);
	Serial.println(" MB");

	// MSC is ready for read/write
	usbMsc.setUnitReady(true);
}

void USB_Drive::writeToFile(const char *path, const uint8_t *buffer, const size_t size)
{
	File file = sd.open(path, O_WRITE | O_CREAT | O_APPEND);
	if (file)
	{
		Serial.printf("Writing data to %s... ", path);
		file.write(buffer, size);
		file.close();
		Serial.println("done.");
	}
	else
	{
		// if the file didn't open, print an error:
		Serial.printf("Error opening %s!\n", path);
	}
}

void USB_Drive::loop()
{
	if (fsChanged)
	{
		fsChanged = false;
		File root = sd.open("/");
		File file;

		Serial.println("SD contents:");

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
	}
}
