#ifndef __OPENSWE1R_WINDOWS_H__
#define __OPENSWE1R_WINDOWS_H__

#include <stdint.h>

#include "emulation.h"

typedef uint32_t MS(DWORD);
typedef void MS(VOID);

typedef uint32_t MS(UINT); // FIXME: Assumption
typedef int16_t MS(SHORT); // FIXME: Assumption

typedef struct {
  uint32_t Data1;
  uint16_t Data2;
  uint16_t Data3;
  uint8_t Data4[8];
} MS(GUID);
typedef MS(GUID) MS(CLSID);
typedef MS(GUID) MS(IID);

typedef struct {
  uint32_t bmType;
  uint32_t bmWidth;
  uint32_t bmHeight;
  uint32_t bmWidthBytes;
  uint16_t bmPlanes;
  uint16_t bmBitsPixel;
  Address bmBits;
} MS(BITMAP);

typedef MS(DWORD) MS(COLORREF);

typedef uint32_t MS(SIZE_T);

typedef struct {
  MS(DWORD) dwLowDateTime;
  MS(DWORD) dwHighDateTime;
} MS(FILETIME);

#define MS__MAX_PATH 260

typedef char MS(TCHAR);
typedef struct {
  MS(DWORD)    dwFileAttributes;
  MS(FILETIME) ftCreationTime;
  MS(FILETIME) ftLastAccessTime;
  MS(FILETIME) ftLastWriteTime;
  MS(DWORD)    nFileSizeHigh;
  MS(DWORD)    nFileSizeLow;
  MS(DWORD)    dwReserved0;
  MS(DWORD)    dwReserved1;
  MS(TCHAR)    cFileName[MS(MAX_PATH)];
  MS(TCHAR)    cAlternateFileName[14];
} MS(WIN32_FIND_DATA);

typedef struct {
  int x;
} MS(STR);
typedef Address MS(LPTSTR);


#define MS__VK_SHIFT 0x10
#define MS__VK_CONTROL 0x11
#define MS__VK_MENU 0x12

#define MS__VK_CAPITAL 0x14

#define MS__VK_LSHIFT 0xA0
#define MS__VK_RSHIFT 0xA1
#define MS__VK_LCONTROL 0xA2
#define MS__VK_RCONTROL 0xA3
#define MS__VK_LMENU 0xA4
#define MS__VK_RMENU 0xA5

#endif
