#ifndef __OPENSWE1R_MAIN_H__
#define __OPENSWE1R_MAIN_H__

#include <unicorn/unicorn.h>

#include "emulation.h"

extern uint32_t callId;

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

// Defines an INITIALIZER macro which will run code at startup
#ifdef __GNUC__
  #define INITIALIZER(_name) \
    __attribute__((constructor)) static void _name()
#elif defined(_MSC_VER)
  #if (_MSC_VER >= 1400)
    #pragma section(".CRT$XIU",long,read)
    #define INITIALIZER(_name) \
      static void _name(); \
      static int __ ## _name ## _caller() { _name(); return 0; } \
      __declspec(allocate(".CRT$XIU")) static int(*__ ## _name ## _pointer)() = __ ## _name ## _caller; \
      static void _name()
  #else
    #error Compiler not supported
  #endif
#else
  #error Compiler not detected
#endif

#define HACKY_IMPORT_BEGIN(_name) \
  static void Hook_ ## _name (void* uc, uint64_t address, uint32_t _size, void* user_data); \
  INITIALIZER(Register_ ## _name) { \
    const char* name = #_name; \
    printf("Registering hook for '%s'\n", name); \
    AddExport(name, Hook_ ## _name, 0); \
  } \
  static void Hook_ ## _name (void* uc, uint64_t _address, uint32_t _size, void* _user_data) { \
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
