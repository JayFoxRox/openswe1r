// This file is a plaground for testing out the game interface

#include "interface.h"

#include <cstdio>
#include <cstdint>

V<uint32_t, 0x50C044> debug_mode;
V<uint32_t, 0xE98EB0> digital_input;
struct foo {
  uint32_t x;
};
S<foo, 0xF00BAA> s;

C<0, uint32_t(uint32_t, float)> foobar;

void printInput() {
  printf("input: 0x%08X\n", (uint32_t)digital_input);
}

#include <thread>
#include <chrono>
void DebugThread() {
  while(true) {
    s->x = 5;
    printf("s->x = %d\n", s->x);
    printInput();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}
void CreateDebugThread() {
  std::thread thread(DebugThread);
}
