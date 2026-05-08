#include <print>

#include "univ/dump.h"

#include "my-cls.h"


int main() {
  MyCls m;

  std::println("dump (client): {}", univ::dump(m));
  MyCls::privileged_print(m);

  std::println("\nformatter (client): {}", m);
}
