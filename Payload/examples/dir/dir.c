#include <stdio.h>
#include <windows.h>

#include "../../psxecute.h"

inline __attribute__((always_inline)) void ListFiles(const char *dir)
{
    PSX_PRINTF_1("Listing directory: %s\n", dir);
    WIN32_FIND_DATA findFileData;
    HANDLE          hFind = FindFirstFileA(dir, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        PSX_PRINTF_1("Failed to find files in directory: %lu\n", GetLastError());
        return;
    }

    do
    {
        ULARGE_INTEGER fileSize;
        fileSize.LowPart = findFileData.nFileSizeLow;
        fileSize.HighPart = findFileData.nFileSizeHigh;
        int fs = fileSize.HighPart;
        if (fs != 0)
            PSX_PRINTF_2("\t%s (Size: %i bytes)\n", findFileData.cFileName, fileSize.HighPart);
        else
            PSX_PRINTF_1("\t%s\n", findFileData.cFileName);
    } while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);
}

int start()
{
    CHAR desktopPath[MAX_PATH];
    CHAR userProfile[MAX_PATH];

    if (GetEnvironmentVariableA("USERPROFILE", userProfile, sizeof(userProfile)) > 0)
    {
        strcpy(desktopPath, userProfile);
        strcat(desktopPath, "\\Desktop\\*");
        ListFiles(desktopPath);
    }
    else
    {
        PSX_PRINT("Failed to get the Desktop path\n");
    }

    ListFiles("C:\\Program Files\\*");
    ListFiles("C:\\Program Files (x86)\\*");
    return 0;
}
