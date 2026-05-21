// Standard C++ headers.
#include <clocale>
#include <iostream>
#include <print>

// Application headers.
#include "building-info.h"
#include "intl-address.h"
#include "my-cls.h"

#include "univ/dump.h"


int main() {
  setlocale(LC_CTYPE, "");

  MyCls m;

  BuildingInfo esb {
      .address = "350 Fifth Avenue",
      .city    = "New York",
      .zip     = 10001,
      .phone   = 212'736'3100,
      .year    = 1931,
  };
  esb.name = "Empire State Building";

  IntlAddress addr {
    .street       = L"Doctor Marañon Ibilbidea, 34",
    .municipality = L"San Sebastián",
    .country      = L"España",
  };

  std::println("MyCls        :: {}", m);
  std::println("BuildingInfo :: {}", esb);
  std::wprintf(L"%S\n", std::format(L"Wide Address :: {}", addr).data());
  std::println();
  

  std::println("dump       : {}", univ::dump(m));
  MyCls::privileged_print(m);
  std::println();

  std::println("dump       : {}", univ::dump(esb));
}
