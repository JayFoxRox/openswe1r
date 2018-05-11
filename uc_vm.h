// Copyright 2018 OpenSWE1R Maintainers
// Licensed under GPLv2 or any later version
// Refer to the included LICENSE.txt file.

#if 0
static void printRegs(uc_engine_kvm* kvm) {
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


static void printRegs(uc_engine_whv* whv) {

  WHV_REGISTER_NAME names[] = {
    WHvX64RegisterRax,
    WHvX64RegisterRbx,
    WHvX64RegisterRcx,
    WHvX64RegisterRdx,
    WHvX64RegisterRsi,
    WHvX64RegisterRdi,
    WHvX64RegisterRsp,
    WHvX64RegisterRbp,
    WHvX64RegisterRip,
    WHvX64RegisterRflags,

    WHvX64RegisterCr0,
    WHvX64RegisterCr2,
    WHvX64RegisterCr3,
    WHvX64RegisterCr4,
    WHvX64RegisterCr8,

    WHvX64RegisterGdtr,
    WHvX64RegisterCs,
    WHvX64RegisterDs,
    WHvX64RegisterEs,
    WHvX64RegisterFs,
    WHvX64RegisterGs,
    WHvX64RegisterSs
  }; 
  struct {
    WHV_REGISTER_VALUE rax;
    WHV_REGISTER_VALUE rbx;
    WHV_REGISTER_VALUE rcx;
    WHV_REGISTER_VALUE rdx;
    WHV_REGISTER_VALUE rsi;
    WHV_REGISTER_VALUE rdi;
    WHV_REGISTER_VALUE rsp;
    WHV_REGISTER_VALUE rbp;
    WHV_REGISTER_VALUE rip;
    WHV_REGISTER_VALUE rflags

    WHV_REGISTER_VALUE cr0;
    WHV_REGISTER_VALUE cr2;
    WHV_REGISTER_VALUE cr3;
    WHV_REGISTER_VALUE cr4;
    WHV_REGISTER_VALUE cr8;

    WHV_REGISTER_VALUE gdt;
    WHV_REGISTER_VALUE cs;
    WHV_REGISTER_VALUE ds;
    WHV_REGISTER_VALUE es;
    WHV_REGISTER_VALUE fs;
    WHV_REGISTER_VALUE gs;
    WHV_REGISTER_VALUE ss;
  } regs;

  HRESULT ret;
  unsigned int count = sizeof(names) / sizeof(WHV_REGISTER_NAME);
  assert((sizeof(regs) / sizeof(WHV_REGISTER_VALUE)) == count);
  ret = WHvGetVirtualProcessorRegisters(whv->partition, whv->vp_index, names, count, &regs);

  printf("rax: 0x%08llx\n", regs.rax.Reg64);
  printf("rbx: 0x%08llx\n", regs.rbx.Reg64);
  printf("rcx: 0x%08llx\n", regs.rcx.Reg64);
  printf("rdx: 0x%08llx\n", regs.rdx.Reg64);
  printf("rsi: 0x%08llx\n", regs.rsi.Reg64);
  printf("rdi: 0x%08llx\n", regs.rdi.Reg64);
  printf("rsp: 0x%08llx\n", regs.rsp.Reg64);
  printf("rbp: 0x%08llx\n", regs.rbp.Reg64);
  printf("rip: 0x%08llx\n", regs.rip.Reg64);
  printf("rflags: 0x%08llx\n", regs.rflags.Reg64);
  printf("=====================\n");
  printf("cr0: 0x%016llx\n", regs.cr0);
  printf("cr2: 0x%016llx\n", regs.cr2);
  printf("cr3: 0x%016llx\n", regs.cr3);
  printf("cr4: 0x%016llx\n", regs.cr4);
  printf("cr8: 0x%016llx\n", regs.cr8);
  printf("gdt: 0x%04x:0x%08llx\n", regs.gdt.Table.Limit, regs.gdt.Table.Base);
  printf("cs: 0x%08llx ds: 0x%08llx es: 0x%08llx\nfs: 0x%08llx gs: 0x%08llx ss: 0x%08llx\n",
       regs.cs.Segment.Base,
       regs.ds.Segment.Base,
       regs.es.Segment.Base,
       regs.fs.Segment.Base,
       regs.gs.Segment.Base,
       regs.ss.Segment.Base);
}
#endif

static void printRegs(uc_engine* uc) {
  printf("<printRegs not implemented>\n");
}
