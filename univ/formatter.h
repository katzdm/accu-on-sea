#ifndef UNIV_FORMATTER_H_
#define UNIV_FORMATTER_H_

#include <format>
#include <meta>
#include <ranges>


namespace univ {
struct formatter {
  struct writer {
    void *out;
    void(*cb)(void *, std::string_view);

    template <typename T>
    writer(T *out) : out(out), cb(write_callback<T>) { }

    void operator()(std::string_view blob) {
      cb(out, blob);
    }

  private:
    template <typename T>
    static void write_callback(void *out, std::string_view blob) {
      *(static_cast<T *>(out)) = blob;
    }
  };

  constexpr auto parse(auto& ctx) { return ctx.begin(); }

  template <typename ObjT>
  void format_impl(const ObjT &o, writer &w) const {
    auto identifier_of_or = [](std::meta::info R, std::string_view alt) {
      return has_identifier(R) ? identifier_of(R) : alt;
    };

    if constexpr (is_pointer_type(^^ObjT)) {
      w(std::format("&{}", *o));
    } else if constexpr (!is_class_type(^^ObjT)) {
      w(std::format("{}", o));
    } else if constexpr (!is_complete_type(^^ObjT)) {
        w(std::format("incomplete {}",
                       identifier_of_or(^^ObjT, "(unnamed-type)")));
    } else {
      w(std::format("{}{{ ", identifier_of_or(^^ObjT, "(unnamed-type)")));
      auto write_separator = [first=true, &w]() mutable {
        if (first)
          first = false;
        else
          w(", ");
      };

      constexpr auto cx = std::meta::access_context::unchecked();
      template for (constexpr auto base :
                    define_static_array(bases_of(^^ObjT, cx))) {
        write_separator();
        w(std::format("{}", (const typename [: type_of(base) :] &)(o)));
      }

      template for (constexpr auto mem :
                    define_static_array(nonstatic_data_members_of(^^ObjT,
                                                                  cx))) {
        write_separator();
        if constexpr (is_bit_field(mem) && !has_identifier(mem)) {
            w("(unnamed-bitfield)");
        } else {
          w(std::format(".{}=", identifier_of_or(mem, "(unnamed-member)")));
          if constexpr (is_pointer_type(type_of(mem)))
            format_impl(o.[:mem:], w);
          else
            w(std::format("{}", o.[:mem:]));
        }
      }
      w(" }");
    }
  }

  template <typename ObjT>
  using format_impl_t = void(const ObjT &, writer &) const;

  template <typename T>
  auto format(const T &o, auto& ctx) const {
    auto out = ctx.out();
    writer w{&out};

    format_impl<T>(o, w);
    return out;
  }
};
}  // namespace univ


#endif  // UNIV_FORMATTER_H_
