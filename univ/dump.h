#ifndef UNIV_DUMP_H_
#define UNIV_DUMP_H_

#include <format>
#include <meta>
#include <ranges>


namespace univ {
namespace detail {

consteval auto 
collect_incompletes_into(auto &inserter, std::meta::info ty) -> void {
  ty = unwrap_ref_decay(remove_pointer(ty));

  if (!is_class_type(ty)) {
    return;
  } else if (is_complete_type(ty)) {
    constexpr auto cx = std::meta::access_context::unchecked();
    for (auto so : std::views::concat(bases_of(ty, cx),
                                      nonstatic_data_members_of(ty, cx)))
      collect_incompletes_into(inserter, type_of(so));
  } else {
    inserter = ty;
  }
}

consteval auto collect_incompletes(std::meta::info ty) {
  std::vector<std::meta::info> result;
  auto inserter = std::back_inserter(result);

  collect_incompletes_into(inserter, ty);
  return std::define_static_array(result);
}
}  // namespace detail

template <typename T, auto Is = detail::collect_incompletes(^^T).data()>
auto dump_to(auto &inserter, const T &o) -> void {
  auto identifier_of_or = [](std::meta::info R, std::string_view alt) {
    return has_identifier(R) ? identifier_of(R) : alt;
  };

  if constexpr (is_pointer_type(^^T)) {
    inserter = '&';
    dump_to<typename [:decay(^^decltype(*o)):], Is>(inserter, *o);
  } else if constexpr (!is_class_type(^^T)) {
    std::format_to(inserter, "{}", o);
  } else if constexpr (!is_complete_type(^^T)) {
      std::format_to(inserter, "incomplete {}", identifier_of(^^T));
  } else {
    std::format_to(inserter, "{}{{ ", identifier_of_or(^^T, "(unnamed-type)"));
    auto write_separator = [first=true, &inserter]() mutable {
      if (first)
        first = false;
      else
        std::ranges::copy(", ", inserter);
    };

    constexpr auto cx = std::meta::access_context::unchecked();
    template for (constexpr auto base :
                  define_static_array(bases_of(^^T, cx))) {
      write_separator();
      using BTy = typename [: type_of(base) :];
      dump_to<BTy, Is>(inserter, (const BTy &)(o));
    }

    template for (constexpr auto mem :
                  define_static_array(nonstatic_data_members_of(^^T, cx))) {
      write_separator();
      if constexpr (is_bit_field(mem) && !has_identifier(mem)) {
          std::ranges::copy("(unnamed-bitfield)", inserter);
      } else {
        std::format_to(inserter,
                       ".{}=", identifier_of_or(mem, "(unnamed-member)"));
        dump_to<typename [:type_of(mem):], Is>(inserter, o.[:mem:]);
      }
    }
    std::ranges::copy(" }", inserter);
  }
}

template <typename T, auto Is = detail::collect_incompletes(^^T).data()>
auto dump(const T &o) -> std::string {
  std::string result;
  auto inserter = std::back_inserter(result);

  dump_to(inserter, o);
  return result;
}
}  // namespace univ

#endif  // UNIV_DUMP_H_
