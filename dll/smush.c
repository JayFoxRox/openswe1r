#include "../export.h"
#include "../emulation.h"
#include "../main.h"

#include <inttypes.h>

HACKY_IMPORT_BEGIN(SmushSetVolume)
  hacky_printf("volume 0x%" PRIX32 "\n", stack[1]);
  eax = 0; // void
  // cdecl
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(SmushPlay)
  hacky_printf("filename 0x%" PRIX32 " ('%s')\n", stack[1], (char*)Memory(stack[1]));
  hacky_printf("arg2 0x%" PRIX32 "\n", stack[2]);
  hacky_printf("arg3 0x%" PRIX32 "\n", stack[3]);
  hacky_printf("arg4 0x%" PRIX32 "\n", stack[4]);
  hacky_printf("arg5 0x%" PRIX32 "\n", stack[5]);
  hacky_printf("width %" PRIu32 "\n", stack[6]);
  hacky_printf("height %" PRIu32 "\n", stack[7]);
  hacky_printf("arg8 0x%" PRIX32 "\n", stack[8]);
  hacky_printf("arg9 0x%" PRIX32 "\n", stack[9]);
  hacky_printf("frameRenderCallback 0x%" PRIX32 "\n", stack[10]);
  hacky_printf("arg11 0x%" PRIX32 "\n", stack[11]);
  hacky_printf("arg12 0x%" PRIX32 "\n", stack[12]);
  hacky_printf("arg13 0x%" PRIX32 "\n", stack[13]);

  eax = 0; // int
  // cdecl
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(SmushShutdown)
  eax = 0; // void
  // cdecl
HACKY_IMPORT_END()

HACKY_IMPORT_BEGIN(SmushStartup)
  hacky_printf("hwnd 0x%" PRIX32 "\n", stack[1]);
  hacky_printf("directSound 0x%" PRIX32 "\n", stack[2]);
  eax = 1; // int
  // cdecl
HACKY_IMPORT_END()

//FIXME: Not used?
HACKY_IMPORT_BEGIN(SmushGetFrameCount)
  eax = 0; // int
  // cdecl
HACKY_IMPORT_END()
