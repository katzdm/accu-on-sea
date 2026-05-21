#ifndef MY_CLS_H_
#define MY_CLS_H_

// Standard C++ headers.
#include <format>

// Application headers.
#include "univ/formatter.h"


// 'MyCls' demonstrates a type that utilizes the "pImpl" pattern (i.e., its
// fields belong to a struct whose definition is only reachable from the .cpp
// file that provides the implementation of 'MyCls').

class MyCls {
  struct impl_t;
  impl_t *impl;

public:
  MyCls();
  ~MyCls();

  // A static member function whose implementation can reach the definition of
  // 'MyCls::impl_t'. A call to 'dump(m)' invokes a specialization of 'dump'
  // that is able to print 'impl'; a separate call to 'dump(m)' in 'main.cpp'
  // is not able to reach the definition of 'MyCls::impl_t', so it prints a
  // token placeholder in lieu of introspecting 'impl'.
  //
  // The specializations invoked by these two calls to 'dump(m)' are distinct,
  // so there is no ODR-violation: the specialization chosen by template
  // argument deduction is sensitive to the context in which the call appears.
  static void privileged_print(const MyCls &m);
};


// The 'std::formatter' class is specialized for both 'MyCls' and for
// 'MyCls::impl_t', using 'univ::formatter'.

template <typename CharT>
struct std::formatter<MyCls, CharT> : univ::formatter { };

template <typename CharT>
struct std::formatter<MyCls::impl_t, CharT> : univ::formatter { };


// The 'MyCls::impl_t'-specializations of 'univ::formatter::format',
// corresponding to the 'format_context' and 'wformat_context' variations, are
// declared to be instantiated elsewhere (i.e., within the file that contains
// the definition of 'MyCls::impl_t').

extern template
univ::formatter::format_t<MyCls::impl_t> univ::formatter::format;

extern template
univ::formatter::wformat_t<MyCls::impl_t> univ::formatter::format;

#endif  // MYCLS_H_
