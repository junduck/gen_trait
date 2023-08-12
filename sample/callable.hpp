#ifndef INCLUDE_5b2080591d29f4e6f36b6e10c1e138cee6b575cf
#define INCLUDE_5b2080591d29f4e6f36b6e10c1e138cee6b575cf

#include <functional>
#include <memory>

namespace jduck::gen_trait::sample {
namespace detail {
template <typename R, typename... Args>
struct callable_base {
  struct vtable_base {
    using _gentrait_invk_t = R (*)(void *, Args...);
    _gentrait_invk_t _gentrait_invk;
    bool operator==(vtable_base const &) const = default;
  };
  struct vtable : vtable_base {
    void (*_gentrait_destroy)(void *);
  };
  template <typename _GENTRAIT_IMPL>
  struct vtable_impl {
    static R _gentrait_invk(void *_gentrait_impl, Args... args) { return static_cast<_GENTRAIT_IMPL *>(_gentrait_impl)->operator()(static_cast<Args &&>(args)...); }
    static void _gentrait_destroy(void *impl) { delete static_cast<_GENTRAIT_IMPL *>(impl); }
  };
  template <typename _GENTRAIT_IMPL>
  constexpr static vtable vtable_for{
      vtable_impl<_GENTRAIT_IMPL>::_gentrait_invk,
      vtable_impl<_GENTRAIT_IMPL>::_gentrait_destroy,
  };
  template <typename _GENTRAIT_IMPL>
  constexpr static bool not_relative = !std::is_base_of_v<callable_base, std::decay_t<_GENTRAIT_IMPL>>;
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
template <typename R, typename... Args>
class callable_ref : detail::callable_base<R, Args...> {
  using base = detail::callable_base<R, Args...>;
  template <typename, typename...>
  friend class callable;
  template <typename, typename...>
  friend class callable_shared;
  friend struct std::hash<callable_ref>;
  void *_gentrait_impl;
  typename base::vtable_base _gentrait_vtbl;
  callable_ref(void *impl, typename base::vtable const *vtbl) noexcept : _gentrait_impl(impl), _gentrait_vtbl(*vtbl) {}

public:
  callable_ref() noexcept = default;
  template <typename _GENTRAIT_IMPL>
  explicit callable_ref(_GENTRAIT_IMPL *impl) noexcept : callable_ref(impl, &base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL>
  explicit callable_ref(std::unique_ptr<_GENTRAIT_IMPL> const &impl) noexcept : callable_ref(impl.get(), &base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL>
  explicit callable_ref(std::shared_ptr<_GENTRAIT_IMPL> const &impl) noexcept : callable_ref(impl.get(), &base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL,
            typename = std::enable_if_t<base::template not_relative<_GENTRAIT_IMPL> &&
                                        base::template not_smartptr<_GENTRAIT_IMPL>>>
  callable_ref(_GENTRAIT_IMPL &impl) noexcept : callable_ref(std::addressof(impl), &base::template vtable_for<_GENTRAIT_IMPL>) {}
  void swap(callable_ref &other) noexcept {
    std::swap(_gentrait_impl, other._gentrait_impl);
    std::swap(_gentrait_vtbl, other._gentrait_vtbl);
  }
  friend void swap(callable_ref &lhs, callable_ref &rhs) noexcept { lhs.swap(rhs); }
  friend bool operator==(callable_ref const &lhs, callable_ref const &rhs) noexcept { return lhs._gentrait_vtbl == rhs._gentrait_vtbl && lhs._gentrait_impl == rhs._gentrait_impl; }

  R operator()(Args... args) const { return _gentrait_vtbl._gentrait_invk(_gentrait_impl, static_cast<Args &&>(args)...); }
};
template <typename R, typename... Args>
class callable : detail::callable_base<R, Args...> {
  using base = detail::callable_base<R, Args...>;
  friend struct std::hash<callable>;
  void *_gentrait_impl;
  typename base::vtable const *_gentrait_vtbl;

public:
  callable(callable const &) = delete;
  callable &operator=(callable const &) = delete;
  callable(callable &&other) noexcept : _gentrait_impl(other._gentrait_impl), _gentrait_vtbl(other._gentrait_vtbl) { other._gentrait_impl = nullptr; }
  callable &operator=(callable &&other) noexcept {
    auto tmp(std::move(other));
    tmp.swap(*this);
    return *this;
  }
  template <typename _GENTRAIT_IMPL>
  explicit callable(_GENTRAIT_IMPL *impl) noexcept : _gentrait_impl(impl), _gentrait_vtbl(&base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL>
  explicit callable(std::unique_ptr<_GENTRAIT_IMPL> impl) noexcept : _gentrait_impl(impl.release()), _gentrait_vtbl(&base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL, typename = std::enable_if_t<base::template not_relative<_GENTRAIT_IMPL>>>
  callable(_GENTRAIT_IMPL &&impl) : _gentrait_impl(new std::remove_reference_t<_GENTRAIT_IMPL>(std::forward<_GENTRAIT_IMPL>(impl))),
                                    _gentrait_vtbl(&base::template vtable_for<std::remove_reference_t<_GENTRAIT_IMPL>>) {}
  ~callable() {
    _gentrait_vtbl->_gentrait_destroy(_gentrait_impl);
    _gentrait_impl = nullptr;
  }
  operator callable_ref<R, Args...>() const { return {_gentrait_impl, _gentrait_vtbl}; }
  void swap(callable &other) noexcept {
    std::swap(_gentrait_impl, other._gentrait_impl);
    std::swap(_gentrait_vtbl, other._gentrait_vtbl);
  }
  friend void swap(callable &lhs, callable &rhs) noexcept { lhs.swap(rhs); }
  friend bool operator==(callable const &lhs, callable const &rhs) noexcept { return lhs._gentrait_vtbl == rhs._gentrait_vtbl && lhs._gentrait_impl == rhs._gentrait_impl; }

  R operator()(Args... args) const { return _gentrait_vtbl->_gentrait_invk(_gentrait_impl, static_cast<Args &&>(args)...); }
};
template <typename R, typename... Args>
class callable_shared : detail::callable_base<R, Args...> {
  using base = detail::callable_base<R, Args...>;
  friend struct std::hash<callable_shared>;
  std::shared_ptr<void> _gentrait_impl;
  typename base::vtable const *_gentrait_vtbl;

public:
  template <typename _GENTRAIT_IMPL>
  explicit callable_shared(_GENTRAIT_IMPL *impl) : _gentrait_impl(impl), _gentrait_vtbl(&base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL>
  explicit callable_shared(std::unique_ptr<_GENTRAIT_IMPL> impl) noexcept : _gentrait_impl(impl.release()), _gentrait_vtbl(&base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL>
  explicit callable_shared(std::shared_ptr<_GENTRAIT_IMPL> impl) noexcept : _gentrait_impl(impl), _gentrait_vtbl(&base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL, typename = std::enable_if_t<base::template not_relative<_GENTRAIT_IMPL>>>
  callable_shared(_GENTRAIT_IMPL &&impl) : _gentrait_impl(std::make_shared<std::remove_reference_t<_GENTRAIT_IMPL>>(std::forward<_GENTRAIT_IMPL>(impl))), _gentrait_vtbl(&base::template vtable_for<std::remove_reference_t<_GENTRAIT_IMPL>>) {}
  operator callable_ref<R, Args...>() const { return {_gentrait_impl.get(), _gentrait_vtbl}; }
  void swap(callable_shared &other) noexcept {
    _gentrait_impl.swap(other._gentrait_impl);
    std::swap(_gentrait_vtbl, other._gentrait_vtbl);
  }
  friend void swap(callable_shared &lhs, callable_shared &rhs) noexcept { lhs.swap(rhs); }
  friend bool operator==(callable_shared const &lhs, callable_shared const &rhs) noexcept { return lhs._gentrait_vtbl == rhs._gentrait_vtbl && lhs._gentrait_impl == rhs._gentrait_impl; }

  R operator()(Args... args) const { return _gentrait_vtbl->_gentrait_invk(_gentrait_impl.get(), static_cast<Args &&>(args)...); }
};
} // namespace jduck::gen_trait::sample

namespace std {
template <typename R, typename... Args>
struct hash<jduck::gen_trait::sample::callable_ref<R, Args...>> {
  size_t operator()(jduck::gen_trait::sample::callable_ref<R, Args...> const &v) const noexcept {
    return std::hash<void *>()(v._gentrait_impl);
  }
};
template <typename R, typename... Args>
struct hash<jduck::gen_trait::sample::callable<R, Args...>> {
  size_t operator()(jduck::gen_trait::sample::callable<R, Args...> const &v) const noexcept {
    return std::hash<void *>()(v._gentrait_impl);
  }
};
template <typename R, typename... Args>
struct hash<jduck::gen_trait::sample::callable_shared<R, Args...>> {
  size_t operator()(jduck::gen_trait::sample::callable_shared<R, Args...> const &v) const noexcept {
    return std::hash<void *>()(v._gentrait_impl.get());
  }
};
} // namespace std
#endif // INCLUDE_5b2080591d29f4e6f36b6e10c1e138cee6b575cf
