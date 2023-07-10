#ifndef INCLUDE_d028f079250f5a43d7a9616de1bf4581703f8013
#define INCLUDE_d028f079250f5a43d7a9616de1bf4581703f8013

#include <functional>
#include <iostream>
#include <memory>

namespace jduck::gen_trait::sample {
namespace detail {
struct drawable_base {
  struct vtable {
    using draw_t = void (*)(void *, std::ostream &);
    draw_t draw;
    using draw_cap_t = void (*)(void *, std::ostream &);
    draw_cap_t draw_cap;
    void (*_gen_trait_destroy)(void *);
  };
  template <typename Impl> struct vtable_impl {
    static void draw(void *impl, std::ostream &os) {
      return static_cast<Impl *>(impl)->draw(os);
    }
    static void draw_cap(void *impl, std::ostream &os) {
      return static_cast<Impl *>(impl)->draw_cap(os);
    }
    static void _gen_trait_destroy(void *impl) {
      delete static_cast<Impl *>(impl);
    }
  };
  template <typename Impl>
  constexpr static vtable vtable_for{
      vtable_impl<Impl>::draw,
      vtable_impl<Impl>::draw_cap,
      vtable_impl<Impl>::_gen_trait_destroy,
  };
  template <typename Impl>
  constexpr static bool not_relative =
      !std::is_base_of_v<drawable_base, std::decay_t<Impl>>;
  template <typename T, template <typename...> typename TA>
  struct is_specialization_of : std::false_type {};
  template <template <typename...> typename T, typename... TA>
  struct is_specialization_of<T<TA...>, T> : std::true_type {};
  template <typename T>
  constexpr static bool not_smartptr =
      !(is_specialization_of<T, std::shared_ptr>::value ||
        is_specialization_of<T, std::unique_ptr>::value);
};
} // namespace detail
class drawable_ref : detail::drawable_base {
  using base = detail::drawable_base;
  friend class drawable;
  friend class drawable_shared;
  friend struct std::hash<drawable_ref>;
  void *impl;
  typename base::vtable const *vtbl;
  drawable_ref(void *impl, typename base::vtable const *vtbl) noexcept
      : impl(impl), vtbl(vtbl) {}

public:
  drawable_ref() noexcept = default;
  template <typename Impl>
  explicit drawable_ref(Impl *impl) noexcept
      : drawable_ref(impl, &base::template vtable_for<Impl>) {}
  template <typename Impl>
  explicit drawable_ref(std::unique_ptr<Impl> const &impl) noexcept
      : drawable_ref(impl.get(), &base::template vtable_for<Impl>) {}
  template <typename Impl>
  explicit drawable_ref(std::shared_ptr<Impl> const &impl) noexcept
      : drawable_ref(impl.get(), &base::template vtable_for<Impl>) {}
  template <typename Impl,
            typename = std::enable_if_t<base::template not_relative<Impl> &&
                                        base::template not_smartptr<Impl>>>
  drawable_ref(Impl &impl) noexcept
      : drawable_ref(std::addressof(impl), &base::template vtable_for<Impl>) {}
  void swap(drawable_ref &other) noexcept {
    std::swap(impl, other.impl);
    std::swap(vtbl, other.vtbl);
  }
  friend void swap(drawable_ref &lhs, drawable_ref &rhs) noexcept {
    lhs.swap(rhs);
  }
  friend bool operator==(drawable_ref const &lhs,
                         drawable_ref const &rhs) noexcept {
    return lhs.vtbl == rhs.vtbl && lhs.impl == rhs.impl;
  }

  void draw(std::ostream &os) { return vtbl->draw(impl, os); }
  void draw_cap(std::ostream &os) const { return vtbl->draw_cap(impl, os); }
};
class drawable : detail::drawable_base {
  using base = detail::drawable_base;
  friend struct std::hash<drawable>;
  void *impl;
  typename base::vtable const *vtbl;

public:
  drawable(drawable const &) = delete;
  drawable &operator=(drawable const &) = delete;
  drawable(drawable &&other) noexcept : impl(other.impl), vtbl(other.vtbl) {
    other.impl = nullptr;
  }
  drawable &operator=(drawable &&other) noexcept {
    auto tmp(std::move(other));
    tmp.swap(*this);
    return *this;
  }
  template <typename Impl>
  explicit drawable(Impl *impl) noexcept
      : impl(impl), vtbl(&base::template vtable_for<Impl>) {}
  template <typename Impl>
  explicit drawable(std::unique_ptr<Impl> impl) noexcept
      : impl(impl.release()), vtbl(&base::template vtable_for<Impl>) {}
  template <typename Impl,
            typename = std::enable_if_t<base::template not_relative<Impl>>>
  drawable(Impl &&impl)
      : impl(new std::remove_reference_t<Impl>(std::forward<Impl>(impl))),
        vtbl(&base::template vtable_for<std::remove_reference_t<Impl>>) {}
  ~drawable() {
    vtbl->_gen_trait_destroy(impl);
    impl = nullptr;
  }
  operator drawable_ref() const { return {impl, vtbl}; }
  void swap(drawable &other) noexcept {
    std::swap(impl, other.impl);
    std::swap(vtbl, other.vtbl);
  }
  friend void swap(drawable &lhs, drawable &rhs) noexcept { lhs.swap(rhs); }
  friend bool operator==(drawable const &lhs, drawable const &rhs) noexcept {
    return lhs.vtbl == rhs.vtbl && lhs.impl == rhs.impl;
  }

  void draw(std::ostream &os) { return vtbl->draw(impl, os); }
  void draw_cap(std::ostream &os) const { return vtbl->draw_cap(impl, os); }
};
class drawable_shared : detail::drawable_base {
  using base = detail::drawable_base;
  friend struct std::hash<drawable_shared>;
  std::shared_ptr<void> impl;
  typename base::vtable const *vtbl;

public:
  template <typename Impl>
  explicit drawable_shared(Impl *impl)
      : impl(impl), vtbl(&base::template vtable_for<Impl>) {}
  template <typename Impl>
  explicit drawable_shared(std::unique_ptr<Impl> impl) noexcept
      : impl(impl.release()), vtbl(&base::template vtable_for<Impl>) {}
  template <typename Impl>
  explicit drawable_shared(std::shared_ptr<Impl> impl) noexcept
      : impl(impl), vtbl(&base::template vtable_for<Impl>) {}
  template <typename Impl,
            typename = std::enable_if_t<base::template not_relative<Impl>>>
  drawable_shared(Impl &&impl)
      : impl(std::make_shared<std::remove_reference_t<Impl>>(
            std::forward<Impl>(impl))),
        vtbl(&base::template vtable_for<std::remove_reference_t<Impl>>) {}
  operator drawable_ref() const { return {impl.get(), vtbl}; }
  void swap(drawable_shared &other) noexcept {
    impl.swap(other.impl);
    std::swap(vtbl, other.vtbl);
  }
  friend void swap(drawable_shared &lhs, drawable_shared &rhs) noexcept {
    lhs.swap(rhs);
  }
  friend bool operator==(drawable_shared const &lhs,
                         drawable_shared const &rhs) noexcept {
    return lhs.vtbl == rhs.vtbl && lhs.impl == rhs.impl;
  }

  void draw(std::ostream &os) { return vtbl->draw(impl.get(), os); }
  void draw_cap(std::ostream &os) const {
    return vtbl->draw_cap(impl.get(), os);
  }
};
} // namespace jduck::gen_trait::sample

namespace std {
template <> struct hash<jduck::gen_trait::sample::drawable_ref> {
  size_t
  operator()(jduck::gen_trait::sample::drawable_ref const &v) const noexcept {
    return std::hash<void *>()(v.impl);
  }
};
template <> struct hash<jduck::gen_trait::sample::drawable> {
  size_t
  operator()(jduck::gen_trait::sample::drawable const &v) const noexcept {
    return std::hash<void *>()(v.impl);
  }
};
template <> struct hash<jduck::gen_trait::sample::drawable_shared> {
  size_t operator()(
      jduck::gen_trait::sample::drawable_shared const &v) const noexcept {
    return std::hash<void *>()(v.impl.get());
  }
};
} // namespace std
#endif // INCLUDE_d028f079250f5a43d7a9616de1bf4581703f8013
