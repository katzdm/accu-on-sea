#ifndef INTL_ADDRESS_H_
#define INTL_ADDRESS_H_

// Standard C++ headers.
#include <string>

// Application headers.
#include "univ/formatter.h"


// 'IntlAddress' demonstrates a class containing 'wstring' fields, which can
// be meaningfully formatted with 'std::wformat'.

struct IntlAddress {
  std::wstring street;
  std::wstring municipality;
  std::wstring country;
};

template <typename CharT>
struct std::formatter<IntlAddress, CharT> : univ::formatter {};

#endif  // INTL_ADDRESS_H_
