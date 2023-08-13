#ifndef INCLUDE_27c2f40de5054500dc1990dcf7abb34563e74c43
#define INCLUDE_27c2f40de5054500dc1990dcf7abb34563e74c43

#include <functional>
#include <iostream>
#include <memory>

namespace jduck::gen_trait::sample {
namespace detail {
struct drawable_base {
  struct vtable_base {
    using _gentrait_fn0_t = void (*)(void *, std::ostream &);
    _gentrait_fn0_t _gentrait_fn0;
    using _gentrait_fn1_t = void (*)(void *, std::ostream &);
    _gentrait_fn1_t _gentrait_fn1;
    bool operator==(vtable_base const &) const = default;
  };
  struct vtable : vtable_base {
    void (*_gentrait_destroy)(void *);
  };
  template <typename _GENTRAIT_IMPL>
  struct vtable_impl {
    static void _gentrait_fn0(void *_gentrait_impl, std::ostream &os) { return static_cast<_GENTRAIT_IMPL *>(_gentrait_impl)->draw(os); }
    static void _gentrait_fn1(void *_gentrait_impl, std::ostream &os) { return static_cast<_GENTRAIT_IMPL *>(_gentrait_impl)->draw_cap(os); }
    static void _gentrait_destroy(void *impl) { delete static_cast<_GENTRAIT_IMPL *>(impl); }
  };
  template <typename _GENTRAIT_IMPL>
  constexpr static vtable vtable_for{
      vtable_impl<_GENTRAIT_IMPL>::_gentrait_fn0,
      vtable_impl<_GENTRAIT_IMPL>::_gentrait_fn1,
      vtable_impl<_GENTRAIT_IMPL>::_gentrait_destroy,
  };
  template <typename _GENTRAIT_IMPL>
  constexpr static bool not_relative = !std::is_base_of_v<drawable_base, std::decay_t<_GENTRAIT_IMPL>>;
  template <typename _GENTRAIT_T, template <typename...> typename _GENTRAIT_TA>
  struct is_specialization_of : std::false_type {};
  template <template <typename...> typename _GENTRAIT_T, typename... _GENTRAIT_TA>
  struct is_specialization_of<_GENTRAIT_T<_GENTRAIT_TA...>, _GENTRAIT_T> : std::true_type {};
  template <typename _GENTRAIT_T>
  constexpr static bool not_smartptr =
      !(is_specialization_of<_GENTRAIT_T, std::shared_ptr>::value ||
        is_specialization_of<_GENTRAIT_T, std::unique_ptr>::value);
};
} // namespace detail
class drawable_ref : detail::drawable_base {
  using base = detail::drawable_base;
  friend class drawable;
  friend class drawable_shared;
  friend struct std::hash<drawable_ref>;
  void *_gentrait_impl;
  typename base::vtable_base _gentrait_vtbl;
  drawable_ref(void *impl, typename base::vtable const *vtbl) noexcept : _gentrait_impl(impl), _gentrait_vtbl(*vtbl) {}

public:
  drawable_ref() noexcept = default;
  template <typename _GENTRAIT_IMPL>
  explicit drawable_ref(_GENTRAIT_IMPL *impl) noexcept : drawable_ref(impl, &base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL>
  explicit drawable_ref(std::unique_ptr<_GENTRAIT_IMPL> const &impl) noexcept : drawable_ref(impl.get(), &base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL>
  explicit drawable_ref(std::shared_ptr<_GENTRAIT_IMPL> const &impl) noexcept : drawable_ref(impl.get(), &base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL,
            typename = std::enable_if_t<base::template not_relative<_GENTRAIT_IMPL> &&
                                        base::template not_smartptr<_GENTRAIT_IMPL>>>
  drawable_ref(_GENTRAIT_IMPL &impl) noexcept : drawable_ref(std::addressof(impl), &base::template vtable_for<_GENTRAIT_IMPL>) {}
  void swap(drawable_ref &other) noexcept {
    std::swap(_gentrait_impl, other._gentrait_impl);
    std::swap(_gentrait_vtbl, other._gentrait_vtbl);
  }
  friend void swap(drawable_ref &lhs, drawable_ref &rhs) noexcept { lhs.swap(rhs); }
  friend bool operator==(drawable_ref const &lhs, drawable_ref const &rhs) noexcept { return lhs._gentrait_vtbl == rhs._gentrait_vtbl && lhs._gentrait_impl == rhs._gentrait_impl; }

  void draw(std::ostream &os) { return _gentrait_vtbl._gentrait_fn0(_gentrait_impl, os); }
  void draw_cap(std::ostream &os) const { return _gentrait_vtbl._gentrait_fn1(_gentrait_impl, os); }
};
class drawable : detail::drawable_base {
  using base = detail::drawable_base;
  friend struct std::hash<drawable>;
  void *_gentrait_impl;
  typename base::vtable const *_gentrait_vtbl;

public:
  drawable(drawable const &) = delete;
  drawable &operator=(drawable const &) = delete;
  drawable(drawable &&other) noexcept : _gentrait_impl(other._gentrait_impl), _gentrait_vtbl(other._gentrait_vtbl) { other._gentrait_impl = nullptr; }
  drawable &operator=(drawable &&other) noexcept {
    auto tmp(std::move(other));
    tmp.swap(*this);
    return *this;
  }
  template <typename _GENTRAIT_IMPL>
  explicit drawable(_GENTRAIT_IMPL *impl) noexcept : _gentrait_impl(impl), _gentrait_vtbl(&base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL>
  explicit drawable(std::unique_ptr<_GENTRAIT_IMPL> impl) noexcept : _gentrait_impl(impl.release()), _gentrait_vtbl(&base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL, typename = std::enable_if_t<base::template not_relative<_GENTRAIT_IMPL>>>
  drawable(_GENTRAIT_IMPL &&impl) : _gentrait_impl(new std::remove_reference_t<_GENTRAIT_IMPL>(std::forward<_GENTRAIT_IMPL>(impl))),
                                    _gentrait_vtbl(&base::template vtable_for<std::remove_reference_t<_GENTRAIT_IMPL>>) {}
  ~drawable() {
    _gentrait_vtbl->_gentrait_destroy(_gentrait_impl);
    _gentrait_impl = nullptr;
  }
  operator drawable_ref() const { return {_gentrait_impl, _gentrait_vtbl}; }
  void swap(drawable &other) noexcept {
    std::swap(_gentrait_impl, other._gentrait_impl);
    std::swap(_gentrait_vtbl, other._gentrait_vtbl);
  }
  friend void swap(drawable &lhs, drawable &rhs) noexcept { lhs.swap(rhs); }
  friend bool operator==(drawable const &lhs, drawable const &rhs) noexcept { return lhs._gentrait_vtbl == rhs._gentrait_vtbl && lhs._gentrait_impl == rhs._gentrait_impl; }

  void draw(std::ostream &os) { return _gentrait_vtbl->_gentrait_fn0(_gentrait_impl, os); }
  void draw_cap(std::ostream &os) const { return _gentrait_vtbl->_gentrait_fn1(_gentrait_impl, os); }
};
class drawable_shared : detail::drawable_base {
  using base = detail::drawable_base;
  friend struct std::hash<drawable_shared>;
  std::shared_ptr<void> _gentrait_impl;
  typename base::vtable const *_gentrait_vtbl;

public:
  template <typename _GENTRAIT_IMPL>
  explicit drawable_shared(_GENTRAIT_IMPL *impl) : _gentrait_impl(impl), _gentrait_vtbl(&base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL>
  explicit drawable_shared(std::unique_ptr<_GENTRAIT_IMPL> impl) noexcept : _gentrait_impl(impl.release()), _gentrait_vtbl(&base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL>
  explicit drawable_shared(std::shared_ptr<_GENTRAIT_IMPL> impl) noexcept : _gentrait_impl(impl), _gentrait_vtbl(&base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL, typename = std::enable_if_t<base::template not_relative<_GENTRAIT_IMPL>>>
  drawable_shared(_GENTRAIT_IMPL &&impl) : _gentrait_impl(std::make_shared<std::remove_reference_t<_GENTRAIT_IMPL>>(std::forward<_GENTRAIT_IMPL>(impl))), _gentrait_vtbl(&base::template vtable_for<std::remove_reference_t<_GENTRAIT_IMPL>>) {}
  operator drawable_ref() const { return {_gentrait_impl.get(), _gentrait_vtbl}; }
  void swap(drawable_shared &other) noexcept {
    _gentrait_impl.swap(other._gentrait_impl);
    std::swap(_gentrait_vtbl, other._gentrait_vtbl);
  }
  friend void swap(drawable_shared &lhs, drawable_shared &rhs) noexcept { lhs.swap(rhs); }
  friend bool operator==(drawable_shared const &lhs, drawable_shared const &rhs) noexcept { return lhs._gentrait_vtbl == rhs._gentrait_vtbl && lhs._gentrait_impl == rhs._gentrait_impl; }

  void draw(std::ostream &os) { return _gentrait_vtbl->_gentrait_fn0(_gentrait_impl.get(), os); }
  void draw_cap(std::ostream &os) const { return _gentrait_vtbl->_gentrait_fn1(_gentrait_impl.get(), os); }
};
} // namespace jduck::gen_trait::sample

namespace std {
template <>
struct hash<jduck::gen_trait::sample::drawable_ref> {
  size_t operator()(jduck::gen_trait::sample::drawable_ref const &v) const noexcept {
    return std::hash<void *>()(v._gentrait_impl);
  }
};
template <>
struct hash<jduck::gen_trait::sample::drawable> {
  size_t operator()(jduck::gen_trait::sample::drawable const &v) const noexcept {
    return std::hash<void *>()(v._gentrait_impl);
  }
};
template <>
struct hash<jduck::gen_trait::sample::drawable_shared> {
  size_t operator()(jduck::gen_trait::sample::drawable_shared const &v) const noexcept {
    return std::hash<void *>()(v._gentrait_impl.get());
  }
};
} // namespace std
#endif // INCLUDE_27c2f40de5054500dc1990dcf7abb34563e74c43
