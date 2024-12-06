#include <windows.h>

#include "../../src/psxecute.h"

int start()
{
    return MessageBoxA(0, GetCommandLineA(), "PSXecute", 0);
}