#include "rom_loader.h"
#include <fstream>
#include <vector>

bool loadRomFromFile(const std::string& path, Bus& bus)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        return false;
    }

    std::streamsize size = file.tellg();
    if (size <= 0)
    {
        return false;
    }
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(static_cast<size_t>(size));
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
    {
        return false;
    }

    bus.loadRom(buffer.data(), buffer.size());
    return true;
}
