#ifndef UNIV_FORMATTER_H_
#define UNIV_FORMATTER_H_

// Standard C++ headers.
#include <algorithm>
#include <format>
#include <meta>
#include <ranges>


namespace univ {

struct formatter;

namespace detail {

struct Subobject {
  std::meta::info Info;
  size_t          Offset;
};

template <typename T>
struct FollowIf {
  using predicate_t = bool(*)(const T &);
  predicate_t predicate;

  constexpr FollowIf() = default;
  constexpr FollowIf(predicate_t p) : predicate(p) {}
};

template <typename T>
FollowIf(typename FollowIf<T>::predicate_t) -> FollowIf<T>;

struct NoFollow {};

constexpr auto access_cx = std::meta::access_context::unchecked();

consteval bool is_universally_formattable(std::meta::info Ty) {
  return std::ranges::any_of(
      bases_of(substitute(^^std::formatter, {Ty}), access_cx),
      [](std::meta::info b) { return type_of(b) == ^^univ::formatter; });
}

consteval std::vector<Subobject> get_nested_objects(std::meta::info Ty,
                                                    size_t offset = 0) {
  std::vector<Subobject> result;
  auto add_directly = [&result, offset](std::meta::info so) {
    result.push_back({so, offset + offset_of(so).bytes});
  };
  auto add_recursive_subobjects = [&](std::meta::info so) {
    result.insert_range(result.end(),
                        get_nested_objects(type_of(so),
                                           offset + offset_of(so).bytes));
  };

  for (auto so : subobjects_of(Ty, access_cx)) {
    if (is_universally_formattable(type_of(so)))
      add_recursive_subobjects(so);
    else
      add_directly(so);
  }

  return result;
}

template <typename CharT>
consteval std::basic_string_view<CharT> to_char_type(auto input) {
  std::basic_string_view contents {input};

  std::basic_string<CharT> result(contents.size(), 0);
  for (size_t idx = 0; auto c : std::basic_string_view{contents})
    result[idx++] = CharT{c};

  return std::define_static_string(result);
}

consteval std::string_view label_of(std::meta::info R) {
  if (is_base(R)) R = type_of(R);

  if (is_type(R))
    return has_identifier(R) ? identifier_of(R) : "(unnamed-type)";
  else
    return has_identifier(R) ? identifier_of(R) :
                               is_bit_field(R) ? "(unnamed-bitfield)"
                                               : "(unnamed-member)";
}

template <typename CharT>
consteval std::basic_string<CharT> format_string(std::meta::info ObjTy) {
  // HELPER LAMBDAS

  auto join = [](auto... ps) -> std::basic_string<CharT> {
    return std::basic_string<CharT> {
        std::from_range,
        std::views::join(std::vector<std::basic_string_view<CharT>>{ps...})
    };
  };

  auto delim = [m=true]() mutable { return m ? (m = false, "") : ", "; };

  // IMPLEMENTATION

  std::basic_string<CharT> result;
  auto affix = [&result, join](auto... ps) -> void {
    std::ranges::copy(join(to_char_type<CharT>(ps)...),
                      std::back_inserter(result));
  };

  if (auto d = dealias(ObjTy);
      has_template_arguments(d) && template_of(d) == ^^std::basic_string) {
    affix(R"("|")");
  } else if (is_pointer_type(ObjTy)) {
    affix("&|");
  } else if (!is_universally_formattable(ObjTy)) {
    affix("|");
  } else {
    affix(label_of(ObjTy), "{");

    std::basic_string<CharT> suffix;
    if (is_class_type(ObjTy)) {
      for (auto so : subobjects_of(ObjTy, access_cx)) {
        if (is_base(so)) {
          if (is_universally_formattable(type_of(so)))
            affix(delim(), format_string<CharT>(type_of(so)));
          else
            affix("|");
        } else {
          affix(delim(),
                ".",
                label_of(so),
                "=",
                format_string<CharT>(type_of(so)));
        }
      }
      affix("}");
    }
  }

  return result;
}

consteval std::meta::info get_format_impl_r(std::meta::info ObjTy,
                                            std::meta::info CtxTy) {
  // Generic lambda forms a template for the printer.
  constexpr auto tmpl = []<typename ObjT,
                           typename CtxT,
                           Subobject... Subobjs>(const ObjT& obj,
                                                 CtxT &cx) static {
    static constexpr std::span fixed_parts = std::define_static_array(
        format_string<typename CtxT::char_type>(^^ObjT) |
        std::views::split(typename CtxT::char_type{'|'}) |
        std::views::transform(
            [](auto piece) { return std::define_static_string(piece); }));

    auto out = cx.out();

    out = fixed_parts[0];
    template for (constexpr size_t k :
                  std::views::iota(0u, sizeof...(Subobjs))) {
      constexpr std::meta::info nso_info = Subobjs...[k].Info;
      constexpr size_t nso_offset = Subobjs...[k].Offset;

      using SubobjTy = [:type_of(nso_info):];
      const SubobjTy &so = reinterpret_cast<const SubobjTy &>(
          *(reinterpret_cast<const char *>(&obj) + nso_offset));

      if constexpr (is_pointer_type(^^SubobjTy)) {
        using pointee_t = std::remove_pointer_t<SubobjTy>;
        constexpr typename std::formatter<pointee_t,
                                          typename CtxT::char_type>::formatter
            formatter;
        if constexpr (size(annotations_of_with_type(nso_info,
                                                    ^^detail::NoFollow)) > 0) {
          out = to_char_type<typename CtxT::char_type>(
              label_of(type_of(nso_info)));
        } else {
          if (so) {
            constexpr auto follow_if_ty = substitute(^^detail::FollowIf,
                                                     {parent_of(nso_info)});
            static constexpr auto follow_if_annots = define_static_array(
                annotations_of_with_type(nso_info, follow_if_ty));
            bool follow = true;
            template for (constexpr std::meta::info annot : follow_if_annots) {
              // Also needs parent offset to really do this right.
              follow = follow && [:constant_of(annot):].predicate(obj);
            }

            if (follow)
              formatter.format(*so, cx);
            else
              out = to_char_type<typename CtxT::char_type>(
                  label_of(remove_pointer(type_of(nso_info))));
          } else {
            out = to_char_type<typename CtxT::char_type>("(null)");
          }
        }
      } else {
        constexpr typename std::formatter<SubobjTy,
                                          typename CtxT::char_type>::formatter
            formatter;
        formatter.format(so, cx);
      }
      out = fixed_parts[k + 1];
    }
    return out;
  };

  std::vector TArgs = {ObjTy, CtxTy};
  TArgs.insert_range(
      TArgs.end(),
      get_nested_objects(ObjTy) |
          std::views::transform(std::meta::reflect_constant<Subobject>));

  // Substitute subobject reflections into 'tmpl' and return the result.
  return substitute(^^decltype(tmpl)::template operator(), TArgs);
};

}  // namespace detail

struct formatter {
  constexpr auto parse(auto& cx) { return cx.begin(); }

  template <typename ObjT, typename CtxT>
  CtxT::iterator format(const ObjT& obj, CtxT& cx) const;

  template <typename ObjT>
  using format_t = std::format_context::iterator(const ObjT &,
                                                 std::format_context &) const;

  template <typename ObjT>
  using wformat_t = std::wformat_context::iterator(const ObjT &,
                                                   std::wformat_context &) const;
};

// Define out-of-line to prevent specializations from being inline.
// Otherwise, declarations of explicit instantations won't suffice to make the
// function a declared specialization, and it will still be subject to implicit
// instantiation.
//
// It pays to be friends with Daveed.
template <typename ObjT, typename CtxT>
CtxT::iterator formatter::format(const ObjT& obj, CtxT& cx) const {
  using namespace ::univ::detail;

  static constexpr auto format_impl = &[:get_format_impl_r(^^ObjT, ^^CtxT):];
  return format_impl(obj, cx);
}

using ::univ::detail::FollowIf;
using ::univ::detail::NoFollow;

}  // namespace univ


#endif  // UNIV_FORMATTER_H_
