#pragma once
#include <Arduino.h>

// Should debug info be printed?
constexpr static bool printDebug = true;
// SD Card Chip Select Pin
constexpr uint32_t SDCARD_CS = 7;
// Interval (in microseconds) for how often temperature measurements should be polled.
constexpr uint32_t tempInterval = 1 * 1000 * 1000;
