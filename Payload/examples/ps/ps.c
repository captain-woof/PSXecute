#include <stdio.h>
#include <tlhelp32.h>
#include <windows.h>

#include "../../src/psxecute.h"

int start()
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        PSX_PRINT("Error creating snapshot\n");
        return 1;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe32))
    {
        PSX_PRINT("Error getting first process\n");
        CloseHandle(hSnapshot);
        return 1;
    }

    PSX_PRINT("Running processes:\n");
    do
    {
        PSX_PRINTF_2("%i\t%s\n", (void*)pe32.th32ProcessID, (void*)pe32.szExeFile);
    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
    return 0;
}
