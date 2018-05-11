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
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/kvm.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>
#include <stropts.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

typedef struct {
  unsigned int mem_slots;
  pthread_t thread;
  int fd;
  int vm_fd;
  int vcpu_fd;
  struct kvm_run *run;
} uc_engine_kvm;

static int _kvm_print_cap(int fd, const char* title, unsigned int cap) {
  int r = ioctl(fd, KVM_CHECK_EXTENSION, cap);
  if (r == -1) {
    fprintf(stderr, "%s: %s\n", title, strerror(errno));
    return -1;
  }
  printf("%s = %d\n", title, r);
  return r;
}
#define kvm_print_cap(kvm, cap) _kvm_print_cap(kvm, #cap, cap)

static void nopSignalHandler() {
  // We don't actually need to do anything here, but we need to interrupt
  // the execution of the guest.
}

uc_err uc_open(uc_arch arch, uc_mode mode, uc_engine **uc) {
  uc_engine_kvm* u = malloc(sizeof(uc_engine_kvm));
  int r;

  u->fd = -1;
  u->vm_fd = -1;
  u->vcpu_fd = -1;
  u->run = NULL;
  u->mem_slots = 0;
  u->thread = pthread_self();

  //FIXME: Only do this on the calling thread
  signal(SIGUSR1, nopSignalHandler); // Prevent termination on USER1 signals

  int fd;
  fd = open("/dev/kvm", O_RDWR);
  if (fd == -1) {
    perror("open /dev/kvm");
    return -1;
  }
  u->fd = fd;

  r = ioctl(u->fd, KVM_GET_API_VERSION, 0);
  assert(r == 12);

  fd = ioctl(u->fd, KVM_CREATE_VM, 0);
  if (fd == -1) {
    fprintf(stderr, "kvm_create_vm: %m\n");
    return -2;
  }
  u->vm_fd = fd;
  
  // Load a small bios which boots CPU into protected mode
  size_t bios_size = 0x1000;
  uint8_t* bios = memalign(0x100000, bios_size);
  FILE* f = fopen("uc_vm_loader", "rb");
  assert(f != NULL);
  fread(bios, 1, bios_size, f);
  fclose(f);

  // Give intel it's required space, I think these addresses are unused.
#if 1
  uint64_t identity_base = 0xFFFFFFFF - (bios_size + 0x4000) + 1; // Needs room for bios + 4 pages
  r = ioctl(u->vm_fd, KVM_SET_IDENTITY_MAP_ADDR, &identity_base); // 1 page
  if (r < 0) {
    fprintf(stderr, "Error assigning Identity Map space: %m\n");
    return -5;
  }
#endif
#if 1
  r = ioctl(u->vm_fd, KVM_SET_TSS_ADDR, identity_base + 0x1000); // 3 pages
  if (r < 0) {
    fprintf(stderr, "Error assigning TSS space: %m\n");
    return -6;
  }
#endif

  r = ioctl(u->vm_fd, KVM_CREATE_VCPU, 0);
  if (r == -1) {
    fprintf(stderr, "kvm_create_vcpu: %m\n");
    return -7;
  }
  u->vcpu_fd = r;

#ifdef KVM_CAP_IMMEDIATE_EXIT
  kvm_print_cap(u->vm_fd, KVM_CAP_IMMEDIATE_EXIT);
#endif
  kvm_print_cap(u->vm_fd, KVM_CAP_NR_VCPUS);
  kvm_print_cap(u->vm_fd, KVM_CAP_MAX_VCPUS);
  kvm_print_cap(u->vm_fd, KVM_CAP_ADJUST_CLOCK);
  kvm_print_cap(u->vm_fd, KVM_CAP_TSC_CONTROL);
  kvm_print_cap(u->vm_fd, KVM_CAP_TSC_DEADLINE_TIMER);
  kvm_print_cap(u->vm_fd, KVM_CAP_READONLY_MEM);
  kvm_print_cap(u->vm_fd, KVM_CAP_SET_IDENTITY_MAP_ADDR);
  kvm_print_cap(u->vm_fd, KVM_CAP_SET_TSS_ADDR);
  kvm_print_cap(u->vm_fd, KVM_CAP_SET_GUEST_DEBUG);
  kvm_print_cap(u->vm_fd, KVM_CAP_IRQCHIP);
  kvm_print_cap(u->vm_fd, KVM_CAP_NR_MEMSLOTS);

  long mmap_size = ioctl(u->fd, KVM_GET_VCPU_MMAP_SIZE, 0);
  if (mmap_size == -1) {
    fprintf(stderr, "get vcpu mmap size: %m\n");
    return -8;
  }
  void *map = mmap(NULL, mmap_size, PROT_READ|PROT_WRITE, MAP_SHARED, u->vcpu_fd, 0);
  if (map == MAP_FAILED) {
    fprintf(stderr, "mmap vcpu area: %m\n");
    return -9;
  }
  u->run = (struct kvm_run*)map;

  // Map bios into address space
  uc_mem_map_ptr(u, 0xFFFFFFFF - bios_size + 1, bios_size, UC_PROT_READ | UC_PROT_EXEC, bios);

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
  ioctl(u->vcpu_fd, KVM_RUN, 0);
  printf("exit reason: %d\n", u->run->exit_reason);
  printRegs(u);
  assert(u->run->exit_reason == KVM_EXIT_HLT);

  // Enable signals
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  r = pthread_sigmask(SIG_UNBLOCK, &set, NULL);
  if (r != 0) {
    fprintf(stderr, "pthread_sigmask %m\n");
  }

  *uc = (uc_engine*)u;
  return 0;
}

uc_err uc_close(uc_engine *uc) {
  uc_engine_kvm* u = (uc_engine_kvm*)uc;
  assert(false);
  //FIXME: Close KVM and shit
  free(uc);
  return 0;
}

uc_err uc_reg_read(uc_engine *uc, int regid, void *value) {
  uc_engine_kvm* u = (uc_engine_kvm*)uc;

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
  uc_engine_kvm* u = (uc_engine_kvm*)uc;

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
  uc_engine_kvm* u = (uc_engine_kvm*)uc;

  //FIXME: set EIP to `begin`

  int r;
  while (1) {
    ioctl(u->vcpu_fd, KVM_RUN, 0);
    switch(u->run->exit_reason) {
    case KVM_EXIT_HLT:
      return 0;
    case KVM_EXIT_IO:
      printRegs(u);
      printf("Error accessing IO\n");
      assert(false);
      return -1;
    case KVM_EXIT_MMIO:
      printRegs(u);
      printf("Error accessing 0x%08X\n", u->run->mmio.phys_addr);
      assert(false);
      return -2;
    case KVM_EXIT_INTR:
      printRegs(u);
      printf("Interrupt\n");
      assert(false);
      return -3;
    case KVM_EXIT_SHUTDOWN:
      printRegs(u);
      printf("Triple fault\n");
      assert(false);
      return -4;
    case KVM_EXIT_FAIL_ENTRY:
      printRegs(u);
      printf("Failed to enter emulation: %llx\n", u->run->fail_entry.hardware_entry_failure_reason);
      assert(false);
      return -5;
    default:
      printRegs(u);
      printf("unhandled exit reason: %i\n", u->run->exit_reason);
      assert(false);
      return -6;
    }
  }
}
uc_err uc_emu_stop(uc_engine *uc) {
  uc_engine_kvm* u = (uc_engine_kvm*)uc;
  pthread_kill(u->thread, SIGUSR1);
  return 0;
}
uc_err uc_mem_map_ptr(uc_engine *uc, uint64_t address, size_t size, uint32_t perms, void *ptr) {
  uc_engine_kvm* u = (uc_engine_kvm*)uc;
  struct kvm_userspace_memory_region memory = {
    .memory_size = size,
    .guest_phys_addr = address,
    .userspace_addr = (uintptr_t)ptr,
    .flags = (perms & UC_PROT_WRITE) ? 0 : KVM_MEM_READONLY, //FIXME: Look at perms?
    .slot = u->mem_slots++,
  };

  printf("Mapping guest 0x%08X - 0x%08X\n", address, address + size - 1);
  
  int r = ioctl(u->vm_fd, KVM_SET_USER_MEMORY_REGION, &memory);
  if (r == -1) {
    fprintf(stderr, "Error mapping memory: %m\n");
    assert(false);
    return -1;
  }

  return 0;
}
const char *uc_strerror(uc_err code) {
  return "meh";
}
