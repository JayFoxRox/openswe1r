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

typedef struct _cbe {
  struct _cbe* next;
  unsigned int hook_index;
  bool removed;
  int type;
  void *callback;
  void *user_data;
  uint64_t begin;
  uint64_t end;
  union {
    int insn; // UC_HOOK_INSN
  } extra;
} cbe; // callback entry

typedef struct {
  unsigned int hook_index;
  unsigned int mem_slots;
  WHV_PARTITION_HANDLE partition;
  unsigned int vp_index;
  cbe* cb_head;
} uc_engine_whv;

static void nopSignalHandler() {
  // We don't actually need to do anything here, but we need to interrupt
  // the execution of the guest.
}

static void printRegs(uc_engine_whv* kvm) {
  struct kvm_regs regs;
  struct kvm_sregs sregs;
  int r = ioctl(kvm->vcpu_fd, KVM_GET_REGS, &regs);
  int s = ioctl(kvm->vcpu_fd, KVM_GET_SREGS, &sregs);
  if (r == -1 || s == -1) {
    fprintf(stderr, "Get Regs failed");
    return;
  }
  printf("rax: 0x%08llx\n", regs.rax);
  printf("rbx: 0x%08llx\n", regs.rbx);
  printf("rcx: 0x%08llx\n", regs.rcx);
  printf("rdx: 0x%08llx\n", regs.rdx);
  printf("rsi: 0x%08llx\n", regs.rsi);
  printf("rdi: 0x%08llx\n", regs.rdi);
  printf("rsp: 0x%08llx\n", regs.rsp);
  printf("rbp: 0x%08llx\n", regs.rbp);
  printf("rip: 0x%08llx\n", regs.rip);
  printf("rflags: 0x%08llx\n", regs.rflags);
  printf("=====================\n");
  printf("cr0: 0x%016llx\n", sregs.cr0);
  printf("cr2: 0x%016llx\n", sregs.cr2);
  printf("cr3: 0x%016llx\n", sregs.cr3);
  printf("cr4: 0x%016llx\n", sregs.cr4);
  printf("cr8: 0x%016llx\n", sregs.cr8);
  printf("gdt: 0x%04x:0x%08llx\n", sregs.gdt.limit, sregs.gdt.base);
  printf("cs: 0x%08llx ds: 0x%08llx es: 0x%08llx\nfs: 0x%08llx gs: 0x%08llx ss: 0x%08llx\n",
       sregs.cs.base, sregs.ds.base, sregs.es.base, sregs.fs.base, sregs.gs.base, sregs.ss.base);
}


uc_err uc_open(uc_arch arch, uc_mode mode, uc_engine **uc) {
  uc_engine_whv* u = malloc(sizeof(uc_engine_whv));
  int r;

  u->hook_index = 0;
  u->fd = -1;
  u->vcpu_fd = -1;
  u->vm_fd = -1;
  u->run = NULL;
  u->cb_head = NULL;
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

uc_err uc_hook_add(uc_engine *uc, uc_hook *hh, int type, void *callback, void *user_data, uint64_t begin, uint64_t end, ...) {
  uc_engine_whv* u = (uc_engine_whv*)uc;

  // Note that the original UC code also does an & comparison here..
  //FIXME: This must scan all flags in proper order

  cbe* cb = malloc(sizeof(cbe));
  cb->removed = false;
  cb->hook_index = u->hook_index++;
  cb->type = type;
  cb->callback = callback;
  cb->user_data = user_data;
  cb->begin = begin;
  cb->end = end;

  if (type & UC_HOOK_INSN) {
    //FIXME: Assert UC_X86_INS_OUT or UC_X86_INS_IN

    va_list valist;

    va_start(valist, end);
    int insn = va_arg(valist, int);
    va_end(valist);

    assert((insn == UC_X86_INS_IN) || (insn == UC_X86_INS_OUT));

    cb->extra.insn = insn;
  } else if (type & UC_HOOK_MEM_READ_UNMAPPED) {
    assert(false); //FIXME: This could be done
  } else if (type & UC_HOOK_MEM_WRITE_UNMAPPED) {
    assert(false); //FIXME: This could be done
  } else if (type & UC_HOOK_MEM_FETCH_UNMAPPED) {
    assert(false); //FIXME: This could be done
  } else if (type & UC_HOOK_MEM_READ_PROT) {
    assert(false); //FIXME: This could be done
  } else if (type & UC_HOOK_MEM_WRITE_PROT) {
    assert(false); //FIXME: This could be done
  } else if (type & UC_HOOK_MEM_FETCH_PROT) {
    assert(false); //FIXME: This could be done
  } else {
    printf("Unsupported hook type: %d\n", type);
    assert(false);
  }

  // Link hook into list
  cb->next = u->cb_head;
  u->cb_head = cb;

  *hh = cb->hook_index;
  return UC_ERR_OK;
}
uc_err uc_hook_del(uc_engine *uc, uc_hook hh) {
  uc_engine_whv* u = (uc_engine_whv*)uc;
  cbe* cb = u->cb_head;
  while(cb != NULL) {
    if (cb->hook_index == hh) {
      cb->removed = true;
      break;
    }
    cb = cb->next;
  }
  return UC_ERR_OK;
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
