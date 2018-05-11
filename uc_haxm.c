// Copyright 2018 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#include <unicorn/unicorn.h>

#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include "uc_vm.h"

#include <hax-interface.h>

#ifdef WIN32

#include <windows.h>

typedef HANDLE hax_fd;
struct hax_state {
  hax_fd fd;
};
#include <hax-windows.h>

static char* hax_path() {
#define HAX_DEVFS "\\\\.\\HAX"
  return strdup(HAX_DEVFS);
}

static char* hax_vm_path(uint32_t vm_id) {
  //assert(vm_id <= MAX_VM_ID);
#define HAX_VM_DEVFS "\\\\.\\hax_vmxx"
  char* name = strdup(HAX_VM_DEVFS);
  snprintf(name, sizeof HAX_VM_DEVFS, "\\\\.\\hax_vm%02d", vm_id);
  return name;
}

static char* hax_vcpu_path(uint32_t vm_id, uint32_t vcpu_id) {
  //assert(vm_id <= MAX_VM_ID);
  //assert(vcpu_id <= MAX_VCPU_ID);
#define HAX_VCPU_DEVFS "\\\\.\\hax_vmxx_vcpuxx"
  char* name = strdup(HAX_VCPU_DEVFS);
  snprintf(name, sizeof HAX_VCPU_DEVFS, "\\\\.\\hax_vm%02d_vcpu%02d", vm_id, vcpu_id);
  return name;
}

static hax_fd hax_open(const char* path) {
  hax_fd fd = CreateFile(path, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (fd == INVALID_HANDLE_VALUE) {
    DWORD errNum = GetLastError();
    if (errno == ERROR_FILE_NOT_FOUND) {
      fprintf(stderr, "HAXM: Interface not found: '%s'\n", path);
    } else {
      fprintf(stderr, "HAXM: Error code %i while initilizing: '%s'\n", errno, path);
    }
    assert(false);
  }
  return fd;
}

static void hax_close(hax_fd fd) {
  CloseHandle(fd);
}

int hax_ioctl(hax_fd fildes, int request, const void* in_data, size_t in_size, void* out_data, size_t out_size) {
  DWORD dSize = 0;
  BOOL ret = DeviceIoControl(fildes, request, in_data, in_size, out_data, out_size, &dSize, (LPOVERLAPPED) NULL);
  assert(dSize == out_size);
  assert(ret != 0);
  return 0;
}

static void* hax_alloc(size_t size) {
  return VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
}

#else

typedef int hax_fd;
struct hax_state {
  hax_fd fd;
};
#include <hax-darwin.h>

static hax_fd hax_open(const char* path) {
  hax_fd fd = open(path, O_RDWR);
  if (fd == -1) {
    fprintf(stderr, "HAXM: Failed to open the hax interface: '%s'\n", path);
  }
  fcntl(fd, F_SETFD, FD_CLOEXEC);
  return fd;
}

static void hax_close(hax_fd fd) {
  close(fd);
}

int hax_ioctl(hax_fd fildes, int request, const void* in_data, size_t in_size, void* out_data, size_t out_size) {
  int ret = ioctl(fildes, request, data);
  return ret;
}

static void* hax_alloc(size_t size) {
  return memalign(0x100000, size);
}

#endif

typedef struct {
  unsigned int mem_slots;
  hax_fd fd;
  hax_fd vm_fd;
  hax_fd vcpu_fd;
  struct hax_tunnel* haxm_tunnel;
  struct hax_tunnel_info haxm_tunnel_info;
} uc_engine_haxm;

static void load_segment(struct segment_desc_t* desc, uint16_t selector, uint64_t base, uint32_t limit, uint16_t ar) {
  desc->selector = selector;
  desc->limit = limit;
  desc->base = base;
  desc->ar = ar;
  return;
}

uc_err uc_open(uc_arch arch, uc_mode mode, uc_engine **uc) {
  uc_engine_haxm* u = malloc(sizeof(uc_engine_haxm));
  int r;

  u->fd = HAX_INVALID_FD;
  u->vm_fd = HAX_INVALID_FD;
  u->vcpu_fd = HAX_INVALID_FD;
  u->haxm_tunnel = NULL;
  u->mem_slots = 0;

  u->fd = hax_open(hax_path());

  struct hax_module_version haxm_version;
  hax_ioctl(u->fd, HAX_IOCTL_VERSION, NULL, 0, &haxm_version, sizeof(haxm_version));
  printf("HAXM Versions: %i, %i\n", haxm_version.compat_version, haxm_version.cur_version);

  struct hax_capabilityinfo cap;
  hax_ioctl(u->fd, HAX_IOCTL_CAPABILITY, NULL, 0, &cap, sizeof(cap));
  assert(cap.wstatus & HAX_CAP_STATUS_WORKING); //FIXME: Should copy qemu error message logic
  //assert(cap.wstatus & HAX_CAP_UG);

  uint32_t vm_id = 0;
  hax_ioctl(u->fd, HAX_IOCTL_CREATE_VM, NULL, 0, &vm_id, sizeof(vm_id));
  u->vm_fd = hax_open(hax_vm_path(vm_id));

  // Expect version 4 of HAXM (same as QEMU at time of writing)
  struct hax_qemu_version qversion;
  qversion.min_version = 4;
  qversion.cur_version = 4;
  hax_ioctl(u->vm_fd, HAX_VM_IOCTL_NOTIFY_QEMU_VERSION, &qversion, sizeof(qversion), NULL, 0);

  // Load a small bios which boots CPU into protected mode
  size_t bios_size = 0x1000;
  uint8_t* bios = hax_alloc(bios_size);
  FILE* f = fopen("uc_vm_loader", "rb");
  assert(f != NULL);
  fread(bios, 1, bios_size, f);
  fclose(f);

  // Map bios into address space
  uc_mem_map_ptr(u, 0xFFFFFFFF - bios_size + 1, bios_size, UC_PROT_READ | UC_PROT_EXEC, bios);
  
  // Create a VCPU
  uint32_t vcpu_id = 0;
  hax_ioctl(u->vm_fd, HAX_VM_IOCTL_VCPU_CREATE, &vcpu_id, sizeof(vcpu_id), NULL, 0);
  u->vcpu_fd = hax_open(hax_vcpu_path(vm_id, vcpu_id));

  hax_ioctl(u->vcpu_fd, HAX_VCPU_IOCTL_SETUP_TUNNEL, NULL, 0, &u->haxm_tunnel_info, sizeof(u->haxm_tunnel_info));
  assert(u->haxm_tunnel_info.size == sizeof(struct hax_tunnel));
  u->haxm_tunnel = (struct hax_tunnel*)u->haxm_tunnel_info.va;



  // Prepare CPU State
  struct vcpu_state_t regs = { 0 };
  r = hax_ioctl(u->vcpu_fd, HAX_VCPU_GET_REGS, NULL, 0, &regs, sizeof(regs));

  regs._rax = 0;
  regs._rbx = 0;
  regs._rcx = 0;
  regs._rdx = 0;
  regs._rsi = 0;
  regs._rdi = 0;
  regs._rsp = 0;
  regs._rbp = 0;
  // FIXME: regs.r8 - regs.r15 ?

  regs._rflags = 2;
  regs._rip = 0xFFF0;

#if 1
  // For restricted guest

#if 0
  db 0x00,0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, // 0
  db 0xff,0xff, 0x00,0x00, 0x00, 0x9b, 0xcf, 0x00  // 1
     0x0000FFFF 0x00CF9B00
                        ^^ base[16:23]
                       ^ access byte [present, privilege, desc, type (executable, direction, rw, accessed)]
                     ^ limit[16:19]
                    ^ flags [granularity, size, longmode, ?]
                  ^^ base [24:31]
           ^^^^ limit[0:15]
       ^^^^ base[0:15]
          
  db 0xff,0xff, 0x00,0x00, 0x00, 0x93, 0xcf, 0x00  // 2
#endif

  regs._cr0 |= 1; // Enable protected mode
  load_segment(&regs._gdt, 0x08, 0xFFFFF000, 0x18, 0x0000);
  load_segment(&regs._cs, 0x08, 0x00000000, 0xFFFFFFFF, 0xCF9B);
  load_segment(&regs._ds, 0x10, 0x00000000, 0xFFFFFFFF, 0xCF93);
  load_segment(&regs._es, 0x10, 0x00000000, 0xFFFFFFFF, 0xCF93);
  load_segment(&regs._ss, 0x10, 0x00000000, 0xFFFFFFFF, 0xCF93);

  regs._rflags = 2;

  // Hack to land us in a HLT
  regs._rip = 0xFFFFFFF0;
  memset(bios, 0xF4, bios_size);

#endif
  r = hax_ioctl(u->vcpu_fd, HAX_VCPU_SET_REGS, &regs, sizeof(regs), NULL, 0);

#if 0
  // Run CPU until it is ready
  printRegs(u);
  hax_ioctl(u->vcpu_fd, HAX_VCPU_IOCTL_RUN, NULL, 0, NULL, 0);

  r = hax_ioctl(u->vcpu_fd, HAX_VCPU_GET_REGS, NULL, 0, &regs, sizeof(regs));

  printf("exit status: %d\n", u->haxm_tunnel->_exit_status);
  printRegs(u);
  assert(u->haxm_tunnel->_exit_status == HAX_EXIT_HLT);
#endif

  *uc = (uc_engine*)u;
  return 0;
}

uc_err uc_close(uc_engine *uc) {
  uc_engine_haxm* u = (uc_engine_haxm*)uc;
  assert(false);
  //FIXME: Close KVM and shit
  free(uc);
  return 0;
}

uc_err uc_reg_read(uc_engine *uc, int regid, void *value) {
  uc_engine_haxm* u = (uc_engine_haxm*)uc;

  assert(u->vcpu_fd != HAX_INVALID_FD);

  struct vcpu_state_t regs = { 0 };
  int r = hax_ioctl(u->vcpu_fd, HAX_VCPU_GET_REGS, NULL, 0, &regs, sizeof(regs));

  if (regid == UC_X86_REG_GDTR) {
    const uc_x86_mmr* gdtr = value;
    //sregs.gdt.base = gdtr->base;
    //sregs.gdt.limit = gdtr->limit;
  } else if (regid == UC_X86_REG_EIP) {
    *(int*)value = regs._rip;
  } else if (regid == UC_X86_REG_ESP) {
    *(int*)value = regs._rsp;
  } else if (regid == UC_X86_REG_EBP) {
    *(int*)value = regs._rbp;
  } else if (regid == UC_X86_REG_ESI) {
    *(int*)value = regs._rsi;
  } else if (regid == UC_X86_REG_EDI) {
    *(int*)value = regs._rdi;
  } else if (regid == UC_X86_REG_EAX) {
    *(int*)value = regs._rax;
  } else if (regid == UC_X86_REG_EBX) {
    *(int*)value = regs._rbx;
  } else if (regid == UC_X86_REG_ECX) {
    *(int*)value = regs._rcx;
  } else if (regid == UC_X86_REG_EDX) {
    *(int*)value = regs._rdx;
  } else if (regid == UC_X86_REG_EFLAGS) {
    *(int*)value = regs._rflags;
  } else {
//    assert(false);
  }

  return 0;
}

uc_err uc_reg_write(uc_engine *uc, int regid, const void *value) {
  int r;
  uc_engine_haxm* u = (uc_engine_haxm*)uc;

  assert(u->vcpu_fd != HAX_INVALID_FD);

  struct vcpu_state_t regs = { 0 };
  r = hax_ioctl(u->vcpu_fd, HAX_VCPU_GET_REGS, NULL, 0, &regs, sizeof(regs));

  if (regid == UC_X86_REG_GDTR) {
    const uc_x86_mmr* gdtr = value;
    //sregs.gdt.base = gdtr->base;
    //sregs.gdt.limit = gdtr->limit;
  } else if (regid == UC_X86_REG_EIP) {
    regs._rip = *(int*)value;
  } else if (regid == UC_X86_REG_ESP) {
    regs._rsp = *(unsigned int*)value;
  } else if (regid == UC_X86_REG_EBP) {
    regs._rbp = *(int*)value;
  } else if (regid == UC_X86_REG_ESI) {
    regs._rsi = *(int*)value;
  } else if (regid == UC_X86_REG_EDI) {
    regs._rdi = *(int*)value;
  } else if (regid == UC_X86_REG_EAX) {
    regs._rax = *(int*)value;
  } else if (regid == UC_X86_REG_EBX) {
    regs._rbx = *(int*)value;
  } else if (regid == UC_X86_REG_ECX) {
    regs._rcx = *(int*)value;
  } else if (regid == UC_X86_REG_EDX) {
    regs._rdx = *(int*)value;
  } else if (regid == UC_X86_REG_EFLAGS) {
    //regs.rflags = *(int*)value;
  } else if (regid == UC_X86_REG_FPSW) {
    //FIXME
  } else if (regid == UC_X86_REG_FPCW) {
    //FIXME
  } else if (regid == UC_X86_REG_FPTAG) {
    //FIXME
  } else if (regid == UC_X86_REG_FP0) {
    //FIXME
  } else if (regid == UC_X86_REG_FP1) {
    //FIXME
  } else if (regid == UC_X86_REG_FP2) {
    //FIXME
  } else if (regid == UC_X86_REG_FP3) {
    //FIXME
  } else if (regid == UC_X86_REG_FP4) {
    //FIXME
  } else if (regid == UC_X86_REG_FP5) {
    //FIXME
  } else if (regid == UC_X86_REG_FP6) {
    //FIXME
  } else if (regid == UC_X86_REG_FP7) {
    //FIXME
  } else if (regid == UC_X86_REG_CS) {

#if 0
  __u64 base;
  __u32 limit;
  __u16 selector;
  __u8  type;
  __u8  present, dpl, db, s, l, g, avl;
  __u8  unusable;
  __u8  padding;
#endif
    //sregs.cs.selector = *(int*)value;
  } else if (regid == UC_X86_REG_DS) {
    //sregs.ds.selector = *(int*)value;
  } else if (regid == UC_X86_REG_ES) {
    //sregs.es.selector = *(int*)value;
  } else if (regid == UC_X86_REG_SS) {
    //sregs.ss.selector = *(int*)value;
  } else if (regid == UC_X86_REG_FS) {
    //sregs.fs.selector = *(int*)value;

  }
  else {
    assert(false);
  }

regs._fs.base = 0xB0000000;
regs._fs.limit = 0x1000;

  r = hax_ioctl(u->vcpu_fd, HAX_VCPU_SET_REGS, &regs, sizeof(regs), NULL, 0);

  return 0;
}

uc_err uc_emu_start(uc_engine *uc, uint64_t begin, uint64_t until, uint64_t timeout, size_t count) {
  int r;
  uc_engine_haxm* u = (uc_engine_haxm*)uc;

  //FIXME: set EIP to `begin`

  while (1) {
    hax_ioctl(u->vcpu_fd, HAX_VCPU_IOCTL_RUN, NULL, 0, NULL, 0);

    struct vcpu_state_t regs = { 0 };
    r = hax_ioctl(u->vcpu_fd, HAX_VCPU_GET_REGS, NULL, 0, &regs, sizeof(regs));

    switch(u->haxm_tunnel->_exit_status) {
    case HAX_EXIT_HLT:
      printf("HLT?! at 0x%08X\n", regs._eip);
      return 0;
    case HAX_EXIT_IO:
      assert(false);
      break;
    case HAX_EXIT_MMIO:
      // Should not happen on UG system
      assert(false);
      break;
    case HAX_EXIT_REAL:
      // Should not happen on UG system
      assert(false);
      break;
    case HAX_EXIT_INTERRUPT:
      assert(false);
      break;
    case HAX_EXIT_UNKNOWN_VMEXIT:
      assert(false);
      break;
    case HAX_EXIT_STATECHANGE:
      printf("HAX_EXIT_STATECHANGE at 0x%08X\n", regs._eip);
      fflush(stdout);
      assert(false);
      break;
    case HAX_EXIT_PAUSED:
      assert(false);
      break;
    case HAX_EXIT_FAST_MMIO:
      assert(false);
      break;
    default:
      printRegs(uc);
      printf("unhandled exit status: %i\n", u->haxm_tunnel->_exit_status);
      assert(false);
      return -6;
    }
  }
}

uc_err uc_emu_stop(uc_engine *uc) {
  uc_engine_haxm* u = (uc_engine_haxm*)uc;
  fprintf(stderr, "Unable to stop haxm!\n"); //FIXME!
  return 0;
}

uc_err uc_mem_map_ptr(uc_engine *uc, uint64_t address, size_t size, uint32_t perms, void *ptr) {
  int r;
  uc_engine_haxm* u = (uc_engine_haxm*)uc;

  struct hax_alloc_ram_info haxm_ram_info = {0};
  haxm_ram_info.va = ptr;
  haxm_ram_info.size = size;
  r = hax_ioctl(u->vm_fd, HAX_VM_IOCTL_ALLOC_RAM, &haxm_ram_info, sizeof(haxm_ram_info), NULL, 0);

  struct hax_set_ram_info haxm_set_ram_info = {0};
  haxm_set_ram_info.pa_start = address;
  haxm_set_ram_info.va = ptr;
  haxm_set_ram_info.size = size;
  haxm_set_ram_info.flags = 0; //FIXME: (perms & UC_PROT_WRITE) ? 0 : HAX_RAM_INFO_ROM;
  r = hax_ioctl(u->vm_fd, HAX_VM_IOCTL_SET_RAM, &haxm_set_ram_info, sizeof(haxm_set_ram_info), NULL, 0);
  if (r == -1) {
    fprintf(stderr, "Error mapping memory: %m\n");
    assert(false);
    return -1;
  }

  printf("Mapping guest 0x%08X - 0x%08X\n", address, address + size - 1);

  return 0;
}

const char *uc_strerror(uc_err code) {
  return "meh";
}
