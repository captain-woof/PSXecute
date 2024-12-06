#include <stdio.h>
#include <windows.h>

#include "../../src/psxecute.h"

int start()
{
    CHAR          szComputerName[MAX_PATH];
    CHAR          szUserName[MAX_PATH];
    OSVERSIONINFO osvi;
    unsigned long maxPath = MAX_PATH;

    if (GetComputerNameA(szComputerName, &maxPath))
    {
        PSX_PRINTF_1("Computer Name: %s\n", szComputerName);
    }
    else
    {
        PSX_PRINTF_1("GetComputerNameA failed with error %lu\n", GetLastError());
    }

    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (GetVersionExA(&osvi))
    {
        PSX_PRINTF_3("OS Version: %lu.%lu.%lu\n", osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber);
    }
    else
    {
        PSX_PRINTF_1("GetVersionExA failed with error %lu\n", GetLastError());
    }

    return 0;
}
