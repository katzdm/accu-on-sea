#include <print>

#include "univ/dump.h"

#include "building-info.h"
#include "my-cls.h"


int main() {
  MyCls m;

  std::println("dump (client): {}", univ::dump(m));
  MyCls::privileged_print(m);

  std::println("\nformatter (client): {}", m);


  BuildingInfo esb { };
  esb.name    = "Empire State Building";
  esb.address = "350 Fifth Avenue";
  esb.city    = "New York";
  esb.zip     = 10001;
  esb.phone   = 212'736'3100;
  esb.year    = 1931;

  std::println("ESB: {}", esb);
}
