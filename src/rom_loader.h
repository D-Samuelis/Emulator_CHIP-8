#pragma once

#include <string>
#include "bus.h"

/**
 * Reads the ROM file at `path` and loads it into `bus`'s program memory.
 * @param path The path to the ROM file to load.
 * @param bus The Bus to load the ROM into.
 * @return true on success, false if the file couldn't be opened or read.
 */
bool loadRomFromFile(const std::string& path, Bus& bus);
