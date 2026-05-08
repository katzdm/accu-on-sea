#ifndef MY_CLS_H_
#define MY_CLS_H_

#include <format>

#include "univ/formatter.h"


class MyCls {
private:
  struct impl_t;
  impl_t *impl;

public:
  MyCls();

  static void privileged_print(const MyCls &m);
};

template <> struct std::formatter<MyCls> : univ::formatter { };
template <> struct std::formatter<MyCls::impl_t> : univ::formatter { };

#endif
