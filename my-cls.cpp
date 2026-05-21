#include "my-cls.h"

// Standard C++ headers.
#include <print>

// Application headers.
#include "univ/dump.h"


struct MyCls::impl_t { int k = 0; };

MyCls::MyCls() : impl(new impl_t{}) {}
MyCls::~MyCls() { delete impl; }

// See comments in 'my-cls.h'
void MyCls::privileged_print(const MyCls &m) {
    std::println("dump (impl): {}", univ::dump(m));
}


// Explicit instantiation definitions of 'univ::formatter::format',
// corresponding to overloads that take a 'std::format_context' and
// 'std::wformat_context', respectively.

template univ::formatter::format_t<MyCls::impl_t> univ::formatter::format;
template univ::formatter::wformat_t<MyCls::impl_t> univ::formatter::format;
