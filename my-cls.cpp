#include "my-cls.h"

// Standard C++ headers.
#include <print>

// Application headers.
#include "univ/dump.h"


struct MyCls::impl_t { int x = 0; int y = 1; };

MyCls::MyCls() : impl(new impl_t{}) {}
MyCls::~MyCls() { delete impl; }

// See comments in 'my-cls.h'
auto MyCls::privileged_dump(const MyCls &m) -> std::string {
    return univ::dump(m);
}


// Explicit instantiation definitions of 'univ::formatter::format',
// corresponding to overloads that take a 'std::format_context' and
// 'std::wformat_context', respectively.

template univ::formatter::format_t<MyCls::impl_t> univ::formatter::format;
template univ::formatter::wformat_t<MyCls::impl_t> univ::formatter::format;
