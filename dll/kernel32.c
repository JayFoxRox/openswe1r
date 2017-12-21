#include "../windows.h"

#include "../export.h"
#include "../emulation.h"
#include "../main.h"

#include <inttypes.h>
#include <assert.h>
#include <string.h>
#include "SDL.h"

static const char** dirlisting = NULL;

static FILE* handles[10000];
static uint32_t handle_index = 1;

static uint32_t tls[1000] = {0};


// 0x8704A0
EXPORT_STDCALL(kernel32, API(DWORD), GetVersion) { // WINAPI
// Windows 98 (https://support.microsoft.com/en-us/kb/189249)
  uint16_t platformId = 1;
  uint8_t majorVersion = 4;
  uint8_t minorVersion = 10;
  return (platformId << 16) | (minorVersion << 8) | majorVersion;
}

// 0x8704F8
EXPORT_STDCALL(kernel32, API(LPTSTR), GetCommandLineA) { //WINAPI
  //return "program"; //FIXME!
  return 0;
}

// 0x8704F4
EXPORT_STDCALL(kernel32, API(VOID), GetStartupInfo, API(LPSTARTUPINFO),lpStartupInfo) { //WINAPI
  //lpStartupInfo->
}


HACKY_IMPORT_BEGIN(GetStdHandle)
  hacky_printf("nStdHandle 0x%" PRIX32 "\n", stack[1]);
  eax = 888;
  esp += 1 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(SetConsoleTextAttribute)
  hacky_printf("hConsoleOutput 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("wAttributes 0x%" PRIX32 "\n", stack[2]);
  eax = 1; // nonzero if succeeds
  esp += 2 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(CreateDirectoryA)
  const char* lpPathName = (const char*)Memory(stack[1]);
  hacky_printf("lpPathName 0x%" PRIX32 " ('%s')\n", stack[1], lpPathName);
  hacky_printf("lpSecurityAttributes 0x%" PRIX32 "\n", stack[2]);
  eax = 1; // nonzero if succeeds
  esp += 2 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(DeleteFileA)
  hacky_printf("lpFileName 0x%" PRIX32 " ('%s')\n", stack[1], (char*)Memory(stack[1]));
  //FIXME: Only stubbed for security reasons
  eax = 1; // nonzero if succeeds
  esp += 1 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(FindNextFileA)
  hacky_printf("hFindFile 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpFindFileData 0x%" PRIX32 "\n", stack[2]);

  if (*dirlisting) {
    API(WIN32_FIND_DATA)* data = Memory(stack[2]);
    data->dwFileAttributes = strchr(*dirlisting,'.') ? 0x80 : 0x10; // FILE_ATTRIBUTE_NORMAL or FILE_ATTRIBUTE_DIRECTORY
    sprintf(data->cFileName, "%s", *dirlisting);
    dirlisting++;
    eax = 1; // File found
  } else {
    eax = 0; // No file found
  }

  esp += 2 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(FindClose)
  hacky_printf("hFindFile 0x%" PRIX32 "\n", stack[1]);
  eax = 1; // nonzero if succeeds
  esp += 1 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(GetModuleHandleA)
  hacky_printf("lpModuleName 0x%" PRIX32 " ('%s')\n", stack[1], Memory(stack[1]));
  eax = 999;
  esp += 1 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(WaitForSingleObject)
  hacky_printf("hHandle 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("dwMilliseconds %" PRId32 "\n", stack[2]);

  eax = 0; // DWORD (0 = "The state of the specified object is signaled.")
  esp += 2 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(CloseHandle)
  hacky_printf("hObject 0x%" PRIX32 "\n", stack[1]);
  if (stack[1] == 5554321) { // Thread handle..
    eax = 1; // nonzero if succeeds
  } else if (stack[1] == 5551337) { // Thread handle..
    eax = 1; // nonzero if succeeds
  } else {
    eax = fclose(handles[stack[1]]) ? 0 : 1; // nonzero if succeeds
  }
  esp += 1 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(CreateEventA)
  hacky_printf("lpEventAttributes 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("bManualReset 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("bInitialState 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("lpName 0x%" PRIX32 " ('%s')\n", stack[4], (char*)Memory(stack[4]));

  eax = 5551337; // HANDLE
  esp += 4 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(CreateThread)
  // Loading in a worker-thread during a loadscreen - Acclaim.. gj.. NOT!
  hacky_printf("lpThreadAttributes 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("dwStackSize 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("lpStartAddress 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("lpParameter 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("dwCreationFlags 0x%" PRIX32 "\n", stack[5]);
  hacky_printf("lpThreadId 0x%" PRIX32 "\n", stack[6]);

  //CreateEmulatedThread(stack[3]);

  eax = 5554321; //  handle to new thread
  esp += 6 * 4;

HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(SetEvent)
  hacky_printf("hEvent 0x%" PRIX32 "\n", stack[1]);

  eax = 1; //  succeeds = return value is nonzero
  esp += 1 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(SetFileAttributesA)
  const char* lpFileName = (const char*)Memory(stack[1]);
  hacky_printf("lpFileName 0x%" PRIX32 " ('%s')\n", stack[1], lpFileName);
  hacky_printf("dwFileAttributes 0x%" PRIX32 "\n", stack[2]);
  eax = 1; // nonzero if succeeds
  esp += 2 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(GetComputerNameA)
  hacky_printf("lpBuffer 0x%" PRIX32 "\n", stack[1]);
  uint32_t* size = (uint32_t*)Memory(stack[2]);
  hacky_printf("lpnSize 0x%" PRIX32 " (%" PRIu32 ")\n", stack[2], *size);
  *size = snprintf(Memory(stack[1]), *size, "ComputerName"); // Cancel was selected
  eax = 1; // nonzero if succeeds
  esp += 2 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(EnterCriticalSection)
  // Heavily impacts performance!
#if 1
  hacky_printf("lpCriticalSection 0x%" PRIX32 "\n", stack[1]);
#else
  silent = true;
#endif
  esp += 1 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(FindFirstFileA)
  const char* pattern = (const char*)Memory(stack[1]);
  hacky_printf("lpFileName 0x%" PRIX32 " ('%s')\n", stack[1], pattern);
  hacky_printf("lpFindFileData 0x%" PRIX32 "\n", stack[2]);
//dwFileAttributes
  char* wildcard1 = strchr(pattern, '*');
  char* wildcard2 = strchr(pattern, '?');
  if ((wildcard1 == NULL) && (wildcard2 == NULL)) {
    // It's asking explicitly for one file..
    static char foundFile[128];
    if ((pattern[0] == '.') && (pattern[1] == '\\')) {
      pattern = &pattern[2];
    } else {
      assert(false);
    }
    strcpy(foundFile, pattern);
    static const char* passthrough[] = {
      ".", "..",
      foundFile,
      NULL
    };
    dirlisting = passthrough;
  } else if (!strcmp(".\\data\\player\\*.sav", pattern)) {
    static const char* profiles[] = {
      ".", "..",
      "anakin.sav",
      NULL
    };
    dirlisting = profiles;
  } else {
    const char* none[] = { NULL };
    dirlisting = none;
    printf("Unknown pattern: '%s'\n", pattern);
    SDL_Delay(3000);
  }

  if (*dirlisting) {
    API(WIN32_FIND_DATA)* data = Memory(stack[2]);
    data->dwFileAttributes = strchr(*dirlisting,'.') ? 0x80 : 0x10; // FILE_ATTRIBUTE_NORMAL or FILE_ATTRIBUTE_DIRECTORY
    sprintf(data->cFileName, "%s", *dirlisting);
    dirlisting++;
    eax = 123; // File found
  } else {
    eax = 0xFFFFFFFF; // INVALID_HANDLE_VALUE = No files found
  }

  esp += 2 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(GetLastError)
  silent = true;
  eax = 0; // no error
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(GetDriveTypeA)
  hacky_printf("lpSubKey 0x%" PRIX32 " ('%s')\n", stack[1], (char*)Memory(stack[1]));
  eax = 5; // Claim everything is CDROM
  esp += 1 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(WriteConsoleA)
  hacky_printf("hConsoleOutput 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpBuffer 0x%" PRIX32 " ('%.*s')\n", stack[2], stack[3], Memory(stack[2]));
  hacky_printf("nNumberOfCharsToWrite 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("lpNumberOfCharsWritten 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("lpReserved 0x%" PRIX32 "\n", stack[5]);
  eax = 1; // nonzero if succeeds
  esp += 5 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(WideCharToMultiByte)
  hacky_printf("CodePage 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("dwFlags 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("lpWideCharStr 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("cchWideChar 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("lpMultiByteStr 0x%" PRIX32 "\n", stack[5]);
  hacky_printf("cbMultiByte 0x%" PRIX32 "\n", stack[6]);
  hacky_printf("lpDefaultChar 0x%" PRIX32 "\n", stack[7]);
  hacky_printf("lpUsedDefaultChar 0x%" PRIX32 "\n", stack[8]);
  eax = 1; //FIXME: Number of chars written
  esp += 8 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(InitializeCriticalSection)
  hacky_printf("lpCriticalSection 0x%" PRIX32 "\n", stack[1]);
  esp += 1 * 4;
HACKY_IMPORT_END()

#if 0
  0x4AC0D8: 0x17E 'InterlockedIncrement' ..missing at 0x0D01C000
#endif

HACKY_IMPORT_BEGIN(VirtualAlloc)
  hacky_printf("lpAddress 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("dwSize 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("flAllocationType 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("flProtect 0x%" PRIX32 "\n", stack[4]);
  eax = Allocate(stack[2]);
  memset(Memory(eax), 0x00, stack[2]);
  esp += 4 * 4;
HACKY_IMPORT_END()

#if 0
  0x4AC0E0: 0x1E5 'RtlUnwind' ..missing at 0x0D01E000
  0x4AC0E4: 0x109 'GetOEMCP' ..missing at 0x0D01F000
#endif

HACKY_IMPORT_BEGIN(GetACP)
  eax = 777; // nonzero if succeeds
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(GetCPInfo)
  hacky_printf("CodePage 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpCPInfo 0x%" PRIX32 "\n", stack[2]);
  eax = 1; // Returns 1 if successful, or 0 otherwise.
  esp += 2 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(GetEnvironmentStringsW)
  Address tmp = Allocate(4);
  ((char*)Memory(tmp))[0] = '\0';
  ((char*)Memory(tmp))[1] = '\0';
  ((char*)Memory(tmp))[2] = '\0';
  ((char*)Memory(tmp))[3] = '\0';
  eax = tmp;
HACKY_IMPORT_END()

#if 0
  0x4AC0F4: 0xE1 'GetEnvironmentStrings' ..missing at 0x0D023000
#endif

HACKY_IMPORT_BEGIN(FreeEnvironmentStringsW)
  hacky_printf("lpszEnvironmentBlock 0x%" PRIX32 "\n", stack[1]);
  eax = 1; // nonzero if succeeds
  esp += 1 * 4;
HACKY_IMPORT_END()

#if 0
  0x4AC0FC: 0x96 'FreeEnvironmentStringsA' ..missing at 0x0D025000
#endif

HACKY_IMPORT_BEGIN(GetModuleFileNameA)
  hacky_printf("hModule 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpFilename 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("nSize 0x%" PRIX32 "\n", stack[3]);
  const char* path = "XYZ";
  assert(stack[3] >= (strlen(path) + 1));
  eax = sprintf((char*)Memory(stack[2]), "%s", path); // number of chars written
  esp += 3 * 4;
HACKY_IMPORT_END()

#if 0
  0x4AC104: 0x250 'UnhandledExceptionFilter' ..missing at 0x0D027000
  0x4AC108: 0x210 'SetEndOfFile' ..missing at 0x0D028000
  0x4AC10C: 0x229 'SetStdHandle' ..missing at 0x0D029000
#endif

HACKY_IMPORT_BEGIN(WriteFile)
  hacky_printf("hFile 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpBuffer 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("nNumberOfBytesToWrite 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("lpNumberOfBytesWritten 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("lpOverlapped 0x%" PRIX32 "\n", stack[5]);
  *(uint32_t*)Memory(stack[4]) = fwrite(Memory(stack[2]), 1, stack[3], handles[stack[1]]);
  eax = 1; // nonzero if succeeds
  esp += 5 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(ReadFile)
  hacky_printf("hFile 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpBuffer 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("nNumberOfBytesToRead 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("lpNumberOfBytesRead 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("lpOverlapped 0x%" PRIX32 "\n", stack[5]);
  *(uint32_t*)Memory(stack[4]) = fread(Memory(stack[2]), 1, stack[3], handles[stack[1]]);
  eax = 1; // nonzero if succeeds
  esp += 5 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(SetFilePointer)
  hacky_printf("hFile 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lDistanceToMove 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("lpDistanceToMoveHigh 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("dwMoveMethod 0x%" PRIX32 "\n", stack[4]);
  int moveMethods[] = { SEEK_SET, SEEK_CUR, SEEK_END };
  assert(stack[4] < 3);
  fseek(handles[stack[1]], stack[2], moveMethods[stack[4]]);
  eax = ftell(handles[stack[1]]);
  //FIXME: Higher word
  esp += 4 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(SetHandleCount)
  hacky_printf("uNumber 0x%" PRIX32 "\n", stack[1]);
  eax = stack[1];
  esp += 1 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(TlsGetValue)
  silent = true;
  if (!silent) {
    hacky_printf("dwTlsIndex 0x%" PRIX32 "\n", stack[1]);
  }
  eax = tls[stack[1]]; // TLS value FIXME!
  esp += 1 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(SetLastError)
  silent = true;
  if (!silent) {
    hacky_printf("dwErrCode 0x%" PRIX32 "\n", stack[1]);
  }
  eax = 0; // TLS value FIXME!
  esp += 1 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(TlsAlloc)
  static int tlsIndex = 0;
  assert(tlsIndex < 500);
  eax = tlsIndex++; // TLS Index
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(TlsSetValue)
  hacky_printf("dwTlsIndex 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpTlsValue 0x%" PRIX32 "\n", stack[2]);
  tls[stack[1]] = stack[2];
  eax = 1; // nonzero if succeeds
  esp += 2 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(GetCurrentThreadId)
  eax = 666; // nonzero if succeeds
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(GetStringTypeW)
  hacky_printf("dwInfoType 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpSrcStr 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("cchSrc 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("lpCharType 0x%" PRIX32 "\n", stack[4]);
  eax = 1; // nonzero if succeeds
  esp += 4 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(LeaveCriticalSection)
  // Heavily impacts performance!
#if 1
  hacky_printf("lpCriticalSection 0x%" PRIX32 "\n", stack[1]);
#else
  silent = true;
#endif
  esp += 1 * 4;
HACKY_IMPORT_END()

#if 0
  0x4AC13C: 0x211 'SetEnvironmentVariableA' ..missing at 0x0D035000
  0x4AC140: 0x1F 'CompareStringW' ..missing at 0x0D036000
  0x4AC144: 0x1E 'CompareStringA' ..missing at 0x0D037000
  0x4AC148: 0xF7 'GetLocaleInfoW' ..missing at 0x0D038000
  0x4AC14C: 0x12B 'GetStringTypeA' ..missing at 0x0D039000
#endif

HACKY_IMPORT_BEGIN(FileTimeToLocalFileTime)
  hacky_printf("lpFileTime 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpLocalFileTime 0x%" PRIX32 "\n", stack[2]);
  //FIXME
  eax = 1; // nonzero if succeeds
  esp += 2 * 4;
HACKY_IMPORT_END()

#if 0
  0x4AC154: 0x25E 'VirtualFree' ..missing at 0x0D03B000
#endif

HACKY_IMPORT_BEGIN(HeapCreate)
  hacky_printf("flOptions 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("dwInitialSize 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("dwMaximumSize 0x%" PRIX32 "\n", stack[3]);
  eax = 0x555;
  esp += 3 * 4;
HACKY_IMPORT_END()

#if 0
  0x4AC15C: 0x16C 'HeapDestroy' ..missing at 0x0D03D000
  0x4AC160: 0x148 'GetTimeZoneInformation' ..missing at 0x0D03E000
  0x4AC164: 0x190 'LoadLibraryA' ..missing at 0x0D03F000
#endif

HACKY_IMPORT_BEGIN(LCMapStringW)
  hacky_printf("Locale 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("dwMapFlags 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("lpSrcStr 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("cchSrc 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("lpDestStr 0x%" PRIX32 "\n", stack[5]);
  hacky_printf("cchDest 0x%" PRIX32 "\n", stack[6]);
//FIXME: MOVE SYMBOLS?!
  eax = 1 + 1; //FIXME: Number of chars in translated string including zero term
  esp += 6 * 4;
HACKY_IMPORT_END()

#if 0
  0x4AC16C: 0x18D 'LCMapStringA' ..missing at 0x0D041000
  0x4AC170: 0x8E 'FlushFileBuffers' ..missing at 0x0D042000
#endif

HACKY_IMPORT_BEGIN(MultiByteToWideChar)
  hacky_printf("CodePage 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("dwFlags 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("lpMultiByteStr 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("cbMultiByte 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("lpWideCharStr 0x%" PRIX32 "\n", stack[5]);
  hacky_printf("cchWideChar 0x%" PRIX32 "\n", stack[6]);
//FIXME: MOVE SYMBOLS?!
  eax = 0; //FIXME: Number of chars written
  esp += 6 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(GetProcAddress)
  Address lpProcName = stack[2];
  const char* procName = Memory(lpProcName);
  hacky_printf("hModule 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpProcName 0x%" PRIX32 " ('%s')\n", lpProcName, procName);

  Export* export = LookupExportByName(procName);
  if (export == NULL) {
    printf("Export for '%s' could not be found\n", procName);
    eax = 0;
    assert(false);
  } else {
    //FIXME: Use existing address for export
    Address hltAddress = CreateHlt();
    AddHltHandler(hltAddress, export->callback, (void*)procName);
    eax = hltAddress;
    printf("Providing at 0x%08X\n", hltAddress);
  }

  esp += 2 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(DeleteCriticalSection)
  hacky_printf("lpCriticalSection 0x%" PRIX32 "\n", stack[1]);
  esp += 1 * 4;
HACKY_IMPORT_END()

#if 0
  0x4AC180: 0x171 'HeapReAlloc' ..missing at 0x0D046000
  0x4AC184: 0x17B 'InterlockedDecrement' ..missing at 0x0D047000
#endif

HACKY_IMPORT_BEGIN(FileTimeToSystemTime)
  hacky_printf("lpFileTime 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("lpSystemTime 0x%" PRIX32 "\n", stack[2]);
  //FIXME
  eax = 1; // nonzero if succeeds
  esp += 2 * 4;
HACKY_IMPORT_END()

#if 0
  0x4AC18C: 0xF6 'GetLocaleInfoA' ..missing at 0x0D049000
#endif

HACKY_IMPORT_BEGIN(HeapFree)
  hacky_printf("hHeap 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("dwFlags 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("lpMem 0x%" PRIX32 "\n", stack[3]);
  eax = 1; // nonzero if succeeds
  esp += 3 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(ExitProcess)
  hacky_printf("uExitCode 0x%" PRIX32 "\n", stack[1]);
  exit(1); //FIXME: Instead, handle this gracefully somehow?!
  esp += 1 * 4;
HACKY_IMPORT_END()

#if 0
  0x4AC198: 0x246 'TerminateProcess' ..missing at 0x0D04C000
  0x4AC19C: 0xD3 'GetCurrentProcess' ..missing at 0x0D04D000
#endif

HACKY_IMPORT_BEGIN(GetVersion)
  hacky_printf("(No parameters)\n");
  eax = 0x00010A04;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(GetStartupInfoA)
  hacky_printf("lpStartupInfo 0x%" PRIX32 "\n", stack[1]);
  esp += 1 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(HeapAlloc)
  hacky_printf("hHeap 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("dwFlags 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("dwBytes 0x%" PRIX32 "\n", stack[3]);
  eax = Allocate(stack[3]);
  //FIXME: Only do this if flag is set..
  memset(Memory(eax), 0x00, stack[3]);
  esp += 3 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(GetFileType)
  hacky_printf("hFile 0x%" PRIX32 "\n", stack[1]);
  //eax = 2; // FILE_TYPE_CHAR
  eax = 1; // FILE_TYPE_DISK
  esp += 1 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(CreateFileA)
  const char* lpFileName = (char*)Memory(stack[1]);
  hacky_printf("lpFileName 0x%" PRIX32 " ('%s')\n", stack[1], lpFileName);
  hacky_printf("dwDesiredAccess 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("dwShareMode 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("lpSecurityAttributes 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("dwCreationDisposition 0x%" PRIX32 "\n", stack[5]);
  hacky_printf("dwFlagsAndAttributes 0x%" PRIX32 "\n", stack[6]);
  hacky_printf("hTemplateFile 0x%" PRIX32 "\n", stack[7]);
  char* path = TranslatePath(lpFileName);
  FILE* f = fopen(path, stack[2] & 0x40000000 ? (stack[5] == 4 ? "ab" : "wb") : "rb");
  if (f != NULL) {
    printf("File handle is 0x%" PRIX32 "\n", handle_index);
    handles[handle_index] = f;
    eax = handle_index;
    handle_index++;
  } else {
    printf("Failed to open file ('%s' as '%s')\n", lpFileName, path);
    eax = 0xFFFFFFFF;
  }
  free(path);
  esp += 7 * 4;
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(GetCommandLineA)
  const char* cmd = "swep1rcr.exe";
  Address tmp = Allocate(strlen(cmd) + 1);
  strcpy((char*)Memory(tmp), cmd);
  eax = tmp;
HACKY_IMPORT_END()
