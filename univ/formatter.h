#ifndef UNIV_FORMATTER_H_
#define UNIV_FORMATTER_H_

#include <format>
#include <meta>
#include <ranges>


namespace univ {
struct formatter {
  constexpr auto parse(auto& ctx) { return ctx.begin(); }

  /*template <typename ObjT, typename CharT>
  void format(*/

  template <typename T>
  auto format(const T &o, auto& ctx) const {
    auto identifier_of_or = [](std::meta::info R, std::string_view alt) {
      return has_identifier(R) ? identifier_of(R) : alt;
    };

    auto out = ctx.out();
    if constexpr (is_pointer_type(^^T)) {
      std::format_to(out, "&{}", *o);
    } else if constexpr (!is_class_type(^^T)) {
      std::format_to(out, "{}", o);
    } else if constexpr (!is_complete_type(^^T)) {
        std::format_to(out, "incomplete {}", identifier_of(^^T));
    } else {
      std::format_to(out, "{}{{ ", identifier_of_or(^^T, "(unnamed-type)"));
      auto write_separator = [first=true, &out]() mutable {
        if (first)
          first = false;
        else
          std::ranges::copy(", ", out);
      };

      constexpr auto cx = std::meta::access_context::unchecked();
      template for (constexpr auto base :
                    define_static_array(bases_of(^^T, cx))) {
        write_separator();
        std::format_to(out, "{}", (const typename [: type_of(base) :] &)(o));
      }

      template for (constexpr auto mem :
                    define_static_array(nonstatic_data_members_of(^^T, cx))) {
        write_separator();
        if constexpr (is_bit_field(mem) && !has_identifier(mem)) {
            std::ranges::copy("(unnamed-bitfield)", out);
        } else {
          std::format_to(out,
                         ".{}=", identifier_of_or(mem, "(unnamed-member)"));
          if constexpr (is_pointer_type(type_of(mem)))
            format(o.[:mem:], ctx);
          else
            std::format_to(out, "{}", o.[:mem:]);
        }
      }
      std::ranges::copy(" }", out);
    }
    return out;
  }
};
}  // namespace univ


#endif  // UNIV_FORMATTER_H_
