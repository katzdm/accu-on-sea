#include <clocale>
#include <iostream>
#include <print>

#include "univ/dump.h"

#include "building-info.h"
#include "intl-address.h"
#include "my-cls.h"


int main() {
  setlocale(LC_CTYPE, "");
  {
    MyCls m;

    std::println("dump (client): {}", univ::dump(m));
    MyCls::privileged_print(m);

    std::println();
    std::println("formatter (client): {}", m);
  }

  {
    BuildingInfo esb {
        .address = "350 Fifth Avenue",
        .city    = "New York",
        .zip     = 10001,
        .phone   = 212'736'3100,
        .year    = 1931,
    };
    esb.name = "Empire State Building";

    std::println();
    std::println("ESB: {}", esb);
  }

  {
    IntlAddress addr {
      .street       = L"Doctor Marañon Ibilbidea, 34",
      .municipality = L"San Sebastián",
      .country      = L"España",
    };

    std::println();
    std::wstring rendered = std::format(L"Wide Address: {}", addr);
    std::wprintf(L"%S\n", rendered.data());
  }
}
