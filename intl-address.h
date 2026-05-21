#ifndef INTL_ADDRESS_H_
#define INTL_ADDRESS_H_

#include <string>

#include "univ/formatter.h"


struct IntlAddress {
  std::wstring street;
  std::wstring municipality;
  std::wstring country;
};

template <typename CharT>
struct std::formatter<IntlAddress, CharT> : univ::formatter {};

#endif  // INTL_ADDRESS_H_
