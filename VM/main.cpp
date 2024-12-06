#include <fstream>

#include "cpu/Psx.h"

std::pair<unsigned char*, size_t> ReadFile(char* filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        printf("Unable to open file\n");
        return std::make_pair(nullptr, 0);
    }

    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    unsigned char* buffer = new unsigned char[fileSize];
    file.read(reinterpret_cast<char*>(buffer), fileSize);
    file.close();
    return std::make_pair(buffer, fileSize);
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Usage: psxecute.exe .\\payload.bin\n");
        return 1;
    }

    std::pair<unsigned char*, size_t> file = ReadFile(argv[1]);
    if (file.second == 0)
        return 1;

    printf("[*] Payload loaded\n");
    PSX psx = PSX(file.first, file.second);

    if (!psx.isInitialized)
    {
        printf("[!] Failed to allocate memory at desired address");
        return 1;
    }
    printf("[*] PSX initialized\n");
    printf("----------------------------------\n");
    auto running = true;
    while (running) running = psx.RunNextInstruction();
    printf("----------------------------------\n");
    printf("[*] Payload returned\n");

    return 0;
}