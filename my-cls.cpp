#include "my-cls.h"

#include <print>

#include "univ/dump.h"


struct MyCls::impl_t { int k = 0; };

MyCls::MyCls() : impl(new impl_t{}) {}

void MyCls::privileged_print(const MyCls &m) {
    std::println("privileged: {}", univ::dump(m));
}
