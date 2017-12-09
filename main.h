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

typedef struct {
  const char* name;
  void(*callback)(void*, Address, void*);
  Address address;
} Export;

Address CreateInterface(const char* name, unsigned int slotCount);
void AddExport(const char* name, void* callback, Address address);
Export* LookupExportByName(const char* name);
Export* LookupExportByOrdinal(const char* name, uint32_t ordinal);

char* TranslatePath(const char* path);

#define HACKY_IMPORT_BEGIN(_name) \
  static void Hook_ ## _name (void* uc, Address address, void* user_data); \
  __attribute__((constructor)) static void Register_ ## _name () { \
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
