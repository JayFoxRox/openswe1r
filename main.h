// Copyright 2017 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

/** 
 * @file
 * This file blabla
 */

#ifndef __OPENSWE1R_MAIN_H__
#define __OPENSWE1R_MAIN_H__

#include <unicorn/unicorn.h>

#include "emulation.h"

extern uint32_t callId;

extern Address clearEax;

static inline int hacky_printf(const char* fmt, ...) {
#if 1
  va_list args;
  va_start(args, fmt);
  int ret = vprintf(fmt, args);
  va_end(args);
  return ret;
#endif
}

Address CreateInterface(const char* name, unsigned int slotCount);
void AddExport(const char* name, void* callback, Address address);

/**
 * Defines an INITIALIZER macro which will run code at startup
 * It can be used like this:
 *
 * ```
 * INITIALIZER(name) {
 *   printf("This function is ran before main!\n");
 * }
 * ```
 */
#if defined(__GNUC__)
#  define INITIALIZER(_name) \
    __attribute__((constructor)) void _name()
#elif defined(_MSC_VER)
#  if (_MSC_VER >= 1400)
#    if !defined(_WIN64)
#      pragma section(".CRT$XIU", read)
#    else
#      pragma section(".CRT$XIU", long, read)
#    endif
#    define INITIALIZER(_name) \
      void _name(); \
      static int __cdecl __ ## _name ## _caller() { _name(); return 0; } \
      __declspec(allocate(".CRT$XIU")) int(__cdecl *__ ## _name ## _pointer)() = __ ## _name ## _caller; \
      void _name()
#  else
#    error Compiler not supported
#  endif
#else
#  error Compiler not detected
#endif

/**
 * This macro creates a function which will be registered in the list of exported API functions at startup.
 * Such functions will later be resolved when an exe is loaded.
 * The created function will retrieve register values of the virtual CPU to variables.
 * It will also pop 4 bytes from the stack.
 * 
 * The following variables will be available in functions created using this macro:
 *
 * - `uint32_t* stack`, a pointer to the stack
 * - `Address stackAddress`, the address of the stack in guest memory on function entry
 * - `Address returnAddress`, the value of stack[0]
 * - `uint32_t eax`, the value of the EAX register, typically used for return values
 * - `uint32_t eip`, the value of the EIP register, the instruction pointer
 * - `uint32_t esp`, the value of the ESP register, the stack pointer
 *
 * The value of `eip` will be equal to `returnAddress`.
 * If the function should return elsewhere, `eip` should be be modified.
 *
 * Functions started by `HACKY_IMPORT_BEGIN` should be ended with `HACKY_IMPORT_END` like this:
 *
 * ```
 * HACKY_IMPORT_BEGIN(name)
 *   printf("Function 'name' was called. Parameter was: 0x%08" PRIX32 "\n", stack[1]);
 *   printf("Returning to 0x%08" PRIX32 "\n", returnAddress);
 * HACKY_IMPORT_END()
 * ```
 */
#define HACKY_IMPORT_BEGIN(_name) \
  static void Hook_ ## _name (void* uc, Address _address, void* _user_data); \
  INITIALIZER(Register_ ## _name) { \
    const char* name = #_name; \
    printf("Registering hook for '%s'\n", name); \
    AddExport(name, Hook_ ## _name, 0); \
  } \
  static void Hook_ ## _name (void* uc, Address _address, void* _user_data) { \
    bool silent = false; \
    \
    int eip; \
    uc_reg_read(uc, UC_X86_REG_EIP, &eip); \
    int esp; \
    uc_reg_read(uc, UC_X86_REG_ESP, &esp); \
    int eax; \
    uc_reg_read(uc, UC_X86_REG_EAX, &eax); \
    \
    Address stackAddress = esp; \
    uint32_t* stack = (uint32_t*)Memory(stackAddress); \
    \
    /* This lists a stack trace. */ \
    /* It's a failed attempt because most functions omit the frame pointer */ \
    /*int ebp; \
    uc_reg_read(uc, UC_X86_REG_EBP, &ebp); \
    StackTrace(ebp, 10, 4); */ \
    \
    /* Pop the return address */ \
    Address returnAddress = stack[0]; \
    eip = returnAddress; \
    esp += 4; \

/**
 * This macro ends a function previously started with `HACKY_IMPORT_BEGIN`.
 * It is responsible for printing debug information and writing CPU register variables back to the virtual CPU.
 */
#define HACKY_IMPORT_END() \
    if (!silent) { \
      hacky_printf("Stack at 0x%" PRIX32 "; returning EAX: 0x%08" PRIX32 "\n", stackAddress, eax); \
      hacky_printf("%7" PRIu32 " Emulation at %X ('%s') from %X\n\n", callId, eip, (char*)_user_data, returnAddress); \
    } \
    callId++; \
    \
    uc_reg_write(uc, UC_X86_REG_ESP, &esp); \
    uc_reg_write(uc, UC_X86_REG_EIP, &eip); \
    uc_reg_write(uc, UC_X86_REG_EAX, &eax); \
  }

#define HACKY_COM_BEGIN(interface, slot) HACKY_IMPORT_BEGIN(interface ## __ ## slot)
#define HACKY_COM_END() HACKY_IMPORT_END()

#endif
