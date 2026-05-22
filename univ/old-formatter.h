#ifndef UNIV_OLD_FORMATTER_H_
#define UNIV_OLD_FORMATTER_H_

// Standard C++ headers.
#include <algorithm>
#include <format>
#include <meta>
#include <ranges>


namespace univ {

struct old_formatter {
  static constexpr auto access_ctx = std::meta::access_context::unchecked();

  constexpr auto parse(auto& ctx) { return ctx.begin(); }

  template <typename ObjT>
  static constexpr std::span<const char * const> format_parts = [] consteval {
    std::vector<const char *> parts;
    auto add_part = [&parts](auto... ps) -> void {
      std::vector<std::string_view> pieces = {ps...};
      if (parts.empty()) {
        pieces.insert(std::begin(pieces), "{");
        pieces.insert(std::begin(pieces),
                      has_identifier(^^ObjT) ? identifier_of(^^ObjT)
                                             : "(unnamed-type)");
      }

      parts.push_back(std::define_static_string(std::views::join(pieces)));
    };

    auto delim = [first=true]() mutable {
      if (!first) {
        return ", ";
      }
      first = false;
      return "";
    };

    std::string nsdms_prefix;
    template for (constexpr auto base :
                  define_static_array(bases_of(^^ObjT, access_ctx))) {
      auto base_parts = format_parts<typename [:type_of(base):]>;

      add_part(delim(), base_parts[0]);
      base_parts = base_parts.subspan(1);

      if (!base_parts.empty()) {
        nsdms_prefix = base_parts.back();
        base_parts = base_parts.first(base_parts.size() - 1);
      }

      std::ranges::copy(base_parts, std::back_inserter(parts));
    }

    bool close_quote = false;
    auto maybe_quote = [](bool b) { return b ? "\"" : ""; };

    for (auto mem : nonstatic_data_members_of(^^ObjT, access_ctx)) {
      add_part(nsdms_prefix,
               maybe_quote(close_quote),
               delim(),
               ".",
               has_identifier(mem) ? identifier_of(mem) :
                                     is_bit_field(mem) ? "(unnamed-bitfield)"
                                                       : "(unnamed-member)",
               "=",
               maybe_quote(close_quote = (type_of(mem) ==
                                          dealias(^^std::string))));
        nsdms_prefix = "";
    }
    add_part(maybe_quote(close_quote), "}");

    return std::define_static_array(parts);
  }();

  template <typename ObjT, typename CtxT>
  static constexpr std::meta::info formatter_info = [] consteval {
    static constexpr std::span fixed_parts = format_parts<ObjT>;

    // Generic lambda forms a template for the printer.
    static constexpr auto impl_tmpl =
        []<std::meta::info... Subobjs>(const ObjT& obj, CtxT &cx) static {
      auto out = cx.out();

      out = fixed_parts[0];
      template for (constexpr size_t k :
                    std::views::iota(0u, sizeof...(Subobjs))) {
        using SubobjTy = [:type_of(Subobjs...[k]):];
        constexpr typename std::formatter<SubobjTy>::formatter formatter;

        formatter.format(obj.[:Subobjs...[k]:], cx);
        out = fixed_parts[k + 1];
      }
      return out;
    };

    std::vector<std::meta::info> TArgs;
    [&TArgs](this auto const& add_nsdms_from,
             std::meta::info Cls) consteval -> void {
      for (auto base : bases_of(Cls, access_ctx))
        add_nsdms_from(type_of(base));
      std::ranges::copy(
          nonstatic_data_members_of(Cls, access_ctx) |
              std::views::transform(
                  std::meta::reflect_constant<std::meta::info>),
          std::back_inserter(TArgs));
    }(^^ObjT);

    // Substitute subobject reflections into eneric lambda and return result.
    return substitute(^^decltype(impl_tmpl)::template operator(), TArgs);
  }();

  template <typename ObjT, typename CtxT>
  auto format(const ObjT& obj, CtxT& cx) const {
    return [:formatter_info<ObjT, CtxT>:](obj, cx);
  }
};

}  // namespace univ


#endif  // UNIV_OLD_FORMATTER_H_
