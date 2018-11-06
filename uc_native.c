// Copyright 2018 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#include <unicorn/unicorn.h>

#include <stdlib.h>
#include <assert.h>

#include <inttypes.h>


typedef struct {
  int pad;
} uc_engine_native;

uc_err uc_hook_add(uc_engine *uc, uc_hook *hh, int type, void *callback, void *user_data, uint64_t begin, uint64_t end, ...) {
  assert(false);
}

uc_err uc_hook_del(uc_engine *uc, uc_hook hh) {
  assert(false);
}

uc_err uc_open(uc_arch arch, uc_mode mode, uc_engine **uc) {
  uc_engine_native* u = malloc(sizeof(uc_engine_native));
  memset(u, 0x00, sizeof(uc_engine_native));

#if 0
  struct sigaction sa;

  sa.sa_flags = SA_SIGSEGV;
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = sigsegv_handler;
  if (sigaction(SIGSEGV, &sa, NULL) == -1) {
    perror("sigaction");
  }
#endif

#if 0
  uc_engine_kvm* u = malloc(sizeof(uc_engine_kvm));
  int r;

  u->hook_index = 0;
  u->fd = -1;
  u->vcpu_fd = -1;
  u->vm_fd = -1;
  u->run = NULL;
  u->cb_head = NULL;
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
  
  // Give intel it's required space, I think these addresses are unused.
  // Needs room for 4 pages
  uint64_t vm_base = 0xFFFFFFFF - 0x4000 + 1;
#if 1
  r = ioctl(u->vm_fd, KVM_SET_IDENTITY_MAP_ADDR, &vm_base); // 1 page
  if (r < 0) {
    fprintf(stderr, "Error assigning Identity Map space: %m\n");
    return -5;
  }
#endif
#if 1
  r = ioctl(u->vm_fd, KVM_SET_TSS_ADDR, vm_base + 0x1000); // 3 pages
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

  // Prepare CPU State
  struct kvm_regs regs = { 0 };
  r = ioctl(u->vcpu_fd, KVM_GET_REGS, &regs);

  regs.rax = 0;
  regs.rbx = 0;
  regs.rcx = 0;
  regs.rdx = 0;
  regs.rsi = 0;
  regs.rdi = 0;
  regs.rsp = 0;
  regs.rbp = 0;
  // FIXME: regs.r8 - regs.r15 ?

  struct kvm_sregs sregs = { 0 };
  r = ioctl(u->vcpu_fd, KVM_GET_SREGS, &sregs);

  sregs.cr0 |= 1; // Enable protected mode
  load_dtable(&sregs.gdt, 0xFFFFF000, 0x18);
  load_segment(&sregs.cs, 0x08, 0x00000000, 0xFFFFFFFF, 0xCF9B);
  load_segment(&sregs.ds, 0x10, 0x00000000, 0xFFFFFFFF, 0xCF93);
  load_segment(&sregs.es, 0x10, 0x00000000, 0xFFFFFFFF, 0xCF93);
  load_segment(&sregs.ss, 0x10, 0x00000000, 0xFFFFFFFF, 0xCF93);

  regs.rflags = 2;


  r = ioctl(u->vcpu_fd, KVM_SET_REGS, &regs);
  r = ioctl(u->vcpu_fd, KVM_SET_SREGS, &sregs);


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
#endif
}
uc_err uc_close(uc_engine *uc) {
  assert(false);
  return 0;
}


uc_err uc_reg_read(uc_engine *uc, int regid, void *value) {
#if 0
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
#endif
}
uc_err uc_reg_write(uc_engine *uc, int regid, const void *value) {
#if 0
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
#endif
  return 0;
}
uc_err uc_emu_start(uc_engine *uc, uint64_t begin, uint64_t until, uint64_t timeout, size_t count) {
  printf("uc_emu_start\n");
  return 0;
}
uc_err uc_emu_stop(uc_engine *uc) {
  assert(false);
}
uc_err uc_mem_map_ptr(uc_engine *uc, uint64_t address, size_t size, uint32_t perms, void *ptr) {
  printf("Mapping guest 0x%08" PRIX64 " - 0x%08" PRIX64 " to %p\n", address, address + size - 1, ptr);
  
  assert(address == ptr);

  return 0;
}
const char *uc_strerror(uc_err code) {
  return "meh";
}
