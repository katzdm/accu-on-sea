// Standard C++ headers.
#include <chrono>
#include <clocale>
#include <iostream>
#include <print>

// Application headers.
#include "building-info.h"
#include "intl-address.h"
#include "my-cls.h"

#include "univ/dump.h"



struct LinkedList {
  static bool follow_next(const LinkedList &ll) {
    return ll.next && !ll.next->first;
  }

  int k;
  [[=univ::FollowIf(follow_next)]] LinkedList *next;
  bool first = false;
};
template <> struct std::formatter<LinkedList> : univ::formatter {};


int main() {
  setlocale(LC_CTYPE, "");


  LinkedList a {1, nullptr, true};
  LinkedList b {2, &a};
  LinkedList c {3, &b};
  LinkedList d {4, &c};
  a.next = &d;
  std::println("a: {}", a);


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

  std::println("fmt MyCls        :: {}", m);
  std::println("fmt BuildingInfo :: {}", esb);
  std::wprintf(L"%S\n", std::format(L"fmt Wide Address :: {}", addr).data());
  std::println();
  

  std::println("dump              :: {}", univ::dump(m));
  std::println("dump (privileged) :: {}", MyCls::privileged_dump(m));
  std::println();

  std::println("dump       : {}", univ::dump(esb));


  /*auto t1 = std::chrono::system_clock::now();
  for (int x = 0; x < 10000000; ++x)
    (void) std::format("Building: {}", esb);
  auto t2 = std::chrono::system_clock::now();

  std::println("Delta: {}", (t2 - t1));*/
}
