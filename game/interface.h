extern "C" {
#include "../emulation.h"
#include "../main.h"
}

// Variables
template <typename t, Address a = 0>
class V {
  t value;
public:
  t operator =(const t& v) {
    return *(t*)Memory(a) = v;
  }
  operator t() const {
    return *(t*)Memory(a);
  }
};

// Structs
template <typename t, Address a = 0>
class S {
public:
  t* operator->() const {
    return (t*)Memory(a);
  }
};

template <Address a, class R, class... Args>
class C {
public:
#if 0
  operator() {
  }
#endif
};
