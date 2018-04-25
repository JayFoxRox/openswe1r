// Copyright 2017 OpenSWE1R Maintainers
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
#include <stropts.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include <haxm/hax_interface.h>

#ifdef WIN32

typedef HANDLE HaxFd;

static char* hax_path() {
#define HAX_DEVFS "\\\\.\\HAX"
  return strdup(HAX_DEVFS);
}

static char* hax_vm_path(uint32_t vm_id) {
  assert(vm_id <= MAX_VM_ID);
#define HAX_VM_DEVFS "\\\\.\\hax_vmxx"
  char* name = strdup(HAX_VM_DEVFS);
  snprintf(name, sizeof HAX_VM_DEVFS, "\\\\.\\hax_vm%02d", vm_id);
  return name;
}

static char* hax_vcpu_path(uint32_t vm_id, uint32_t vcpu_id) {
  assert(vm_id <= MAX_VM_ID);
  assert(vcpu_id <= MAX_VCPU_ID);

#define HAX_VCPU_DEVFS "\\\\.\\hax_vmxx_vcpuxx"
  char* name = strdup(HAX_VCPU_DEVFS);
  snprintf(name, sizeof HAX_VCPU_DEVFS, "\\\\.\\hax_vm%02d_vcpu%02d", vm_id, vcpu_id);
  return name;
}

static HaxFd hax_open(const char* path) {
  HaxFd fd = CreateFile(path, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (fd == INVALID_HANDLE_VALUE) {
    DWORD errNum = GetLastError();
    if (errno == ERROR_FILE_NOT_FOUND) {
      std::fprintf(stderr, "HAXM: Interface not found: '%s'\n", path);
    } else {
      std::fprintf(stderr, "HAXM: Error code %i while initilizing: '%s'\n", errno, path);
    }
    assert(false);
  }
  return fd;
}

static void hax_close(HaxFd fd) {
  CloseHandle(fd);
}

int hax_ioctl(HaxFd fildes, int request, void* data, unsigned int size) {
  DWORD dSize = 0;
  int ret = DeviceIoControl(filedes, request, NULL, 0, data, size, &dSize, (LPOVERLAPPED) NULL);
  return ret;
}

#else

typedef int HaxFd;

static HaxFd hax_open(const char* path) {
  HaxFd fd = open(path, O_RDWR);
  if (fd == -1) {
    std::fprintf(stderr, "HAXM: Failed to open the hax interface: '%s'\n", path);
  } else {
    fcntl(fd, F_SETFD, FD_CLOEXEC);
    initialized = true;
  }
  return fd;
}

static void hax_close(HaxFd fd) {
  close(fd);
}

int hax_ioctl(HaxFd fildes, int request, void* data, unsigned int size) {
  int ret = ioctl(filedes, request, data);
  return ret;
}

#endif

typedef struct {
  unsigned int mem_slots;
  HaxFd fd;
  HaxFd vm_fd;
  HaxFd vcpu_fd;
  hax_tunnel* haxm_tunnel;
  hax_tunnel_info haxm_tunnel_info;
} uc_engine_haxm;

uc_err uc_open(uc_arch arch, uc_mode mode, uc_engine **uc) {
  uc_engine_haxm* u = malloc(sizeof(uc_engine_haxm));
  int r;

  u->hook_index = 0;
  u->fd = -1;
  u->vcpu_fd = -1;
  u->vm_fd = -1;
  u->run = NULL;
  u->mem_slots = 0;

  u->fd = hax_open(hax_path());

  hax_module_version haxm_version;
  hax_ioctl(u->fd, HAX_IOCTL_VERSION, &haxm_version);
  printf("HAXM Versions: %i, %i\n", haxm_version->compat_version, haxm_version->cur_version);

  uint32_t vm_id = 0;
  hax_ioctl(u->fd, HAX_IOCTL_CREATE_VM, &vm_id, sizeof(vm_id));
  vm_fd = hax_open(hax_vm_path(vm_id));

  // Expect version 4 of HAXM (same as QEMU at time of writing)
  hax_qemu_version qversion;
  qversion.min_version = 4;
  qversion.cur_version = 4;
  hax_ioctl(u->fd, HAX_VM_IOCTL_NOTIFY_QEMU_VERSION, &qversion, sizeof(qversion));

#ifdef _WIN32
  //FIXME: Not sure if this way of allocating memory works fine for HAXM
  ram = VirtualAlloc(NULL, ram_size, MEM_COMMIT, PAGE_READWRITE);
  rom = VirtualAlloc(NULL, rom_size, MEM_COMMIT, PAGE_READWRITE);
#else
  assert(false);
#endif

  // Load a small bios which boots CPU into protected mode
  uint8_t* bios = memalign(0x100000, bios_size);
  FILE* f = fopen("uc_vm_loader", "rb");
  assert(f != NULL);
  fread(bios, 1, bios_size, f);
  fclose(f);

  // Map bios into memory
  uc_mem_map_ptr(uc, 0xFFFFFFFF - bios_size + 1, bios_size, UC_PROT_READ | UC_PROT_EXEC, bios);
  
  // Create a VCPU
  uint32_t vcpu_id = 0;
  hax_ioctl(u->vm_fd, HAX_VM_IOCTL_VCPU_CREATE, &vcpu_id, sizeof(vcpu_id));
  devfs_path = hax_vcpu_devfs_string(vmid, vcpuid);
  u->vcpu_fd = hax_open(hax_vcpu_path(vm_id, vcpu_id));

  hax_ioctl(HAX_VCPU_IOCTL_SETUP_TUNNEL, &u->haxm_tunnel_info, sizeof(u->haxm_tunnel_info));
  assert(haxm_tunnel_info->size == sizeof(hax_tunnel));
  u->haxm_tunnel = haxm_tunnel_info->va;



  // Prepare CPU State
  struct vcpu_state_t regs = { 0 };

  regs.rax = 0;
  regs.rbx = 0;
  regs.rcx = 0;
  regs.rdx = 0;
  regs.rsi = 0;
  regs.rdi = 0;
  regs.rsp = 0;
  regs.rbp = 0;
  // FIXME: regs.r8 - regs.r15 ?

  regs.rflags = 2;
  regs.rip = 0xFFF0;
  r = hax_ioctl(u->fd, HAX_VCPU_SET_REGS, &regs);

  // Run CPU until it is ready
  printRegs(u);
  hax_ioctl(u->fd, HAX_VCPU_IOCTL_RUN, NULL, 0);
  printf("exit reason: %d\n", u->tunnel->exit_reason);
  printRegs(u);
  assert(u->tunnel->exit_reason == HAX_EXIT_HLT);

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

  assert(u->vcpu_fd != -1);

  struct vcpu_state_t regs = { 0 };
  int r = hax_ioctl(u->fd, HAXM_VCPU_GET_REGS, &regs, sizeof(regs));

  if (regid == UC_X86_REG_EIP) {
    *(int*)value = regs.rip;
  } else if (regid == UC_X86_REG_ESP) {
    *(int*)value = regs.rsp;
  } else if (regid == UC_X86_REG_EAX) {
    *(int*)value = regs.rax;
  } else {
//    assert(false);
  }

  return 0;
}
uc_err uc_reg_write(uc_engine *uc, int regid, const void *value) {
  int r;
  uc_engine_haxm* u = (uc_engine_haxm*)uc;

  assert(u->vcpu_fd != -1);

  struct vcpu_state_t regs = { 0 };
  r = hax_ioctl(u->fd, HAXM_VCPU_GET_REGS, &regs, sizeof(regs));

  if (regid == UC_X86_REG_GDTR) {
    const uc_x86_mmr* gdtr = value;
    //sregs.gdt.base = gdtr->base;
    //sregs.gdt.limit = gdtr->limit;
  } else if (regid == UC_X86_REG_EIP) {
    regs.rip = *(int*)value;
  } else if (regid == UC_X86_REG_ESP) {
    regs.rsp = *(unsigned int*)value;
  } else if (regid == UC_X86_REG_EBP) {
    regs.rbp = *(int*)value;
  } else if (regid == UC_X86_REG_ESI) {
    regs.rsi = *(int*)value;
  } else if (regid == UC_X86_REG_EDI) {
    regs.rdi = *(int*)value;
  } else if (regid == UC_X86_REG_EAX) {
    regs.rax = *(int*)value;
  } else if (regid == UC_X86_REG_EBX) {
    regs.rbx = *(int*)value;
  } else if (regid == UC_X86_REG_ECX) {
    regs.rcx = *(int*)value;
  } else if (regid == UC_X86_REG_EDX) {
    regs.rdx = *(int*)value;
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

sregs.fs.base = 0xB0000000;
sregs.fs.limit = 0x1000;

  r = hax_ioctl(u->fd, HAXM_VCPU_SET_REGS, &regs, sizeof(regs));

  return 0;
}
uc_err uc_emu_start(uc_engine *uc, uint64_t begin, uint64_t until, uint64_t timeout, size_t count) {
  uc_engine_haxm* u = (uc_engine_haxm*)uc;

  //FIXME: set EIP to `begin`

  int r;
  while (1) {
    hax_ioctl(u->vcpu_fd, HAX_VCPU_IOCTL_RUN, NULL, 0);
    switch(u->tunnel->_exit_status) {
    case HAX_EXIT_HLT:
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
      assert(false);
      break;
    case HAX_EXIT_PAUSED:
      assert(false);
      break;
    case HAX_EXIT_FAST_MMIO:
      assert(false);
      break;
    default:
      printRegs(u);
      printf("unhandled exit reason: %i\n", u->run->exit_reason);
      assert(false);
      return -6;
    }
  }
}
uc_err uc_emu_stop(uc_engine *uc) {
  uc_engine_haxm* u = (uc_engine_haxm*)uc;
  printf(stderr, "Unable to stop haxm!\n"); //FIXME!
  return 0;
}
uc_err uc_mem_map_ptr(uc_engine *uc, uint64_t address, size_t size, uint32_t perms, void *ptr) {
  uc_engine_haxm* u = (uc_engine_haxm*)uc;

  hax_alloc_ram_info haxm_ram_info = {0};
  haxm_ram_info.va = address;
  haxm_ram_info.size = size;
  hax_ioctl(u->fd, HAX_VM_IOCTL_ALLOC_RAM, &haxm_ram_info, sizeof(haxm_ram_info));

  hax_set_ram_info haxm_set_ram_info = {0};
  haxm_set_ram_info.pa_start = address;
  haxm_set_ram_info.va = ptr;
  haxm_set_ram_info.size = size;
//FIXME: perms!
  haxm_set_ram_info.flags = (perms & UC_PROT_WRITE) 0 : HAX_RAM_INFO_ROM;
  int r = hax_ioctl(u->fd, HAX_VM_IOCTL_SET_RAM, &haxm_set_ram_info, sizeof(haxm_set_ram_info));
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
