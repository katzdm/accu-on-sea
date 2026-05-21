#ifndef MY_CLS_H_
#define MY_CLS_H_

#include <format>

#include "univ/formatter.h"


class MyCls {
  struct impl_t;
  impl_t *impl;

public:
  MyCls();

  static void privileged_print(const MyCls &m);
};

template <typename CharT>
struct std::formatter<MyCls, CharT> : univ::formatter { };

template <typename CharT>
struct std::formatter<MyCls::impl_t, CharT> : univ::formatter { };

extern template
univ::formatter::format_t<MyCls::impl_t> univ::formatter::format;

extern template
univ::formatter::wformat_t<MyCls::impl_t> univ::formatter::format;

#endif  // MYCLS_H_
