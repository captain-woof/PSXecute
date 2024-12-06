#include <windows.h>

#include "../../src/psxecute.h"

int start()
{
    PSX_PRINT("Starting calc.exe\n");

    const char *calcPath = "C:\\Windows\\System32\\calc.exe";

    PROCESS_INFORMATION processInfo = {0};
    STARTUPINFO         startupInfo = {0};

    startupInfo.cb = sizeof(startupInfo);

    if (CreateProcess(
            calcPath,      // Application path
            NULL,          // Command line arguments
            NULL,          // Process attributes
            NULL,          // Thread attributes
            FALSE,         // Inherit handles
            0,             // Creation flags
            NULL,          // Environment variables
            NULL,          // Current directory
            &startupInfo,  // Startup info
            &processInfo   // Process info
        ))
    {
        PSX_PRINT("calc.exe started successfully.\n");
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
    }
    else
    {
        PSX_PRINT("Failed to start calc.exe.");
        return 1;
    }
    PSX_PRINT("Done\n");

    return 0;
}
