//#include <stdio.h>
#include <windows.h>

#include "../psxecute.h"

int start() {
    PSX_PRINT("Entered start function\n");

	STARTUPINFOA startupInfo = {
		.cb = sizeof(STARTUPINFOA)
	};
    PSX_PRINT("Startup info created\n");

	PROCESS_INFORMATION procInfo = {0};
    PSX_PRINT("Process info created\n");

	if (CreateProcessA(
		"C:\\Windows\\System32\\notepad.exe",
		"",
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&startupInfo,
		&procInfo
	)) {
        // Success
		//CHAR result[100] = "";
		//sprintf_s(result, 100, "notepad.exe started; PID: %d, TID: %d", procInfo.dwProcessId, procInfo.dwThreadId);
		//PSX_PRINT(result);
        PSX_PRINT("Process started\n");

        if (procInfo.hProcess != NULL) CloseHandle(procInfo.hProcess);
	    if (procInfo.hThread != NULL) CloseHandle(procInfo.hThread);

        return 0;
	}
	else {
		// Fail
        PSX_PRINT("Failed to launch notepad.exe\n");

        return 1;
	}
}