// Copyright 2018 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#include <unicorn/unicorn.h>

#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>
#include <stropts.h>
#include <signal.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include <windows.h>

typedef struct {
  unsigned int mem_slots;
  WHV_PARTITION_HANDLE partition;
  UINT32 vp_index;
} uc_engine_whv;



uc_err uc_open(uc_arch arch, uc_mode mode, uc_engine **uc) {
  uc_engine_whv* u = malloc(sizeof(uc_engine_whv));
  int r;

  u->fd = -1;
  u->vcpu_fd = -1;
  u->vm_fd = -1;
  u->run = NULL;
  u->mem_slots = 0;

  HRESULT ret;

  ret = WHvCreatePartition(&u->partition);

  u->vp_index = 0;
  ret = WHvCreateVirtualProcessor(u->partition, u->vp_index, 0);

  // Load a small bios which boots CPU into protected mode
  uint8_t* bios = memalign(0x100000, bios_size);
  FILE* f = fopen("uc_vm_loader", "rb");
  assert(f != NULL);
  fread(bios, 1, bios_size, f);
  fclose(f);

  ret = WHvMapGpaRange(u->partition, bios, 0xFFFFFFFF - bios_size + 1, bios_size, WHvMapGpaRangeFlagRead | WHvMapGpaRangeFlagExecute);

  // Prepare CPU State
  struct kvm_regs regs = { 0 };

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
  r = ioctl(u->vcpu_fd, KVM_SET_REGS, &regs);

  // Run CPU until it is ready
  printRegs(u);
  WHV_RUN_VP_EXIT_CONTEXT exit_context;
  ret = WHvRunVirtualProcessor(u->partition, u->vp_index, &exit_context, sizeof(exit_context));
  printRegs(u);
  assert(exit_context.ExitReason == WHvRunVpExitReasonX64Halt);

  *uc = (uc_engine*)u;
  return 0;
}
uc_err uc_close(uc_engine *uc) {
  uc_engine_whv* u = (uc_engine_whv*)uc;
  assert(false);
  //FIXME: Close WHV and shit
  free(uc);
  return 0;
}

uc_err uc_reg_read(uc_engine *uc, int regid, void *value) {
  uc_engine_whv* u = (uc_engine_whv*)uc;

  assert(u->vcpu_fd != -1);

  struct kvm_regs regs;
  struct kvm_sregs sregs;
  int r = ioctl(u->vcpu_fd, KVM_GET_REGS, &regs);
  int s = ioctl(u->vcpu_fd, KVM_GET_SREGS, &sregs);

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
  uc_engine_whv* u = (uc_engine_whv*)uc;

  assert(u->vcpu_fd != -1);

   struct kvm_regs regs;
  struct kvm_sregs sregs;
  int r = ioctl(u->vcpu_fd, KVM_GET_REGS, &regs);
  int s = ioctl(u->vcpu_fd, KVM_GET_SREGS, &sregs);

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

  ioctl(u->vcpu_fd, KVM_SET_REGS, &regs);
  ioctl(u->vcpu_fd, KVM_SET_SREGS, &sregs);

  return 0;
}
uc_err uc_emu_start(uc_engine *uc, uint64_t begin, uint64_t until, uint64_t timeout, size_t count) {
  uc_engine_whv* u = (uc_engine_whv*)uc;

  int r;
  while (1) {
    HRESULT ret;

    WHV_RUN_VP_EXIT_CONTEXT exit_context;
    ret = WHvRunVirtualProcessor(u->partition, u->vp_index, &exit_context, sizeof(exit_context));

    switch(exit_context.ExitReason) {
      case WHvRunVpExitReasonX64Halt:
        return 0;
      case WHvRunVpExitReasonX64IoPortAccess:
        printRegs(u);
        printf("Error accessing IO\n");
        assert(false);
        return -1;
      case WHvRunVpExitReasonMemoryAccess:
        printRegs(u);
        printf("Error accessing 0x%08X\n", exit_context.MemoryAccess.Gpa);
        assert(false);
        return -2;
      case WHvRunVpExitReasonX64InterruptWindow:
        printRegs(u);
        printf("Interrupt window\n");
        assert(false);
        return -3;
      default:
        printRegs(u);
        printf("unhandled exit reason: %i\n", u->run->exit_reason);
        assert(false);
        return -6;
    }
  }
}
uc_err uc_emu_stop(uc_engine *uc) {
  uc_engine_whv* u = (uc_engine_whv*)uc;
  HRESULT ret;
  ret = WHvCancelRunVirtualProcessor(u->parition, u->vp_index, 0);
  return 0;
}
uc_err uc_mem_map_ptr(uc_engine *uc, uint64_t address, size_t size, uint32_t perms, void *ptr) {
  uc_engine_whv* u = (uc_engine_whv*)uc;

  printf("Mapping guest 0x%08X - 0x%08X\n", address, address + size - 1);

  WHV_MAP_GPA_RANGE_FLAGS flags = 0;
  flags |= WHvMapGpaRangeFlagRead;
  flags |= WHvMapGpaRangeFlagExecute;
  if (perms & UC_PROT_WRITE) {
    flags |= WHvMapGpaRangeFlagWrite;
  }

  HRESULT ret;
  ret = WHvMapGpaRange(u->partition, ptr, address, size, flags);

  return 0;
}
const char *uc_strerror(uc_err code) {
  return "meh";
}
