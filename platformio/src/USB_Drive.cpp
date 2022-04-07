#include <USB_Drive.h>

/*
 * constexpr function to get size of static array at compile time.
 */
template <typename T, size_t N> constexpr size_t array_size(T (&)[N])
{
	return N;
}

// Create an array of data structures and fill it with correct flash settings
constexpr uint32_t INTERNAL_FLASH_FILESYSTEM_SIZE = (192 * 1024);
constexpr uint32_t INTERNAL_FLASH_FILESYSTEM_START_ADDR = (0x00040000 - 256 - 0 - INTERNAL_FLASH_FILESYSTEM_SIZE);

Adafruit_InternalFlash USB_Drive::flash(INTERNAL_FLASH_FILESYSTEM_START_ADDR, INTERNAL_FLASH_FILESYSTEM_SIZE);

FatFileSystem USB_Drive::fatfs;
bool USB_Drive::fsFormatted = false;
bool USB_Drive::fsChanged = false;

void USB_Drive::begin()
{
	/*
	 * Callback invoked when received READ10 command.
	 * Copy disk's data to buffer (up to bufsize) and return number of copied bytes (must be multiple of block size).
	 */
	auto msc_read = [](uint32_t lba, void *buffer, uint32_t bufsize) {
		// SPIFlash Block API includes 4K sector caching internally.
		bool read = flash.readBlocks(lba, (uint8_t *)buffer, bufsize / 512);
		return read ? (int32_t)bufsize : -1;
	};

	/*
	 * Callback invoked when received WRITE10 command.
	 * Process data in buffer to disk's storage and return number of written bytes (must be multiple of block size).
	 */
	auto msc_write = [](uint32_t lba, uint8_t *buffer, uint32_t bufsize) {
		// SPIFlash Block API includes 4K sector caching internally.
		bool write = flash.writeBlocks(lba, buffer, bufsize / 512) ? bufsize : -1;
		return write ? (int32_t)bufsize : -1;
	};

	/*
	 * Callback invoked when WRITE10 command is completed (status received and accepted by host).
	 * Used to flush any pending cache.
	 */
	auto msc_flush = [](void) {
		// sync with flash
		flash.syncBlocks();
		// clear file system's cache to force refresh
		fatfs.cacheClear();
		fsChanged = true;
	};

	// Give the flash driver detals about our flash chip
	// flash.begin(flashDevices, array_size(flashDevices));
	flash.begin();
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

void USB_Drive::writeToFile(const char *path, const uint8_t *buffer, const size_t size)
{
	File file = fatfs.open(path, O_WRITE | O_CREAT | O_APPEND);
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

static FatFile root;
static FatFile file;

void USB_Drive::loop()
{
	if (fsChanged)
	{
		fsChanged = false;

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
	}
}
