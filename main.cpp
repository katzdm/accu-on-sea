#include <print>

#include "univ/dump.h"

#include "my-cls.h"


int main() {
  MyCls m;

  std::println("dump m: {}", univ::dump(m));
  MyCls::privileged_print(m);

  std::println("\nformatter: {}", m);
}
