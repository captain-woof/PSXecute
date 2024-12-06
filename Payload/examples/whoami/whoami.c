#include <windows.h>
#define SECURITY_WIN32
#include <security.h>
#include <stdio.h>

#define NAM_SAM_COMPAT 2

#include "../../src/psxecute.h"

int start()
{
    char*         fullName = (char*)malloc(256);
    unsigned long fullNameLen = 256;

    GetUserNameExA(NAM_SAM_COMPAT, fullName, &fullNameLen);
    PSX_PRINT(fullName);
    PSX_PRINT("\n");
    return 0;
}