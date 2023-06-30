#ifndef INCLUDE_ee38b9e9b823fab6fd28013aa63d40aaf378a051
#define INCLUDE_ee38b9e9b823fab6fd28013aa63d40aaf378a051

#include <memory>

namespace jduck::gen_trait::sample {
namespace detail {
template <typename R, typename... Args> struct callable_base {
  struct vtable {
    using invoke_t = R (*)(void *, Args...);
    invoke_t invoke;
    void (*destroy)(void *);
  };
  template <typename Impl> struct vtable_impl {
    static R invoke(void *impl, Args... args) {
      return static_cast<Impl *>(impl)->operator()(
          static_cast<Args &&>(args)...);
    }
    static void destroy(void *impl) { delete static_cast<Impl *>(impl); }
  };
  template <typename Impl>
  constexpr static vtable vtable_for{
      vtable_impl<Impl>::invoke,
      vtable_impl<Impl>::destroy,
  };
  template <typename Impl>
  constexpr static bool not_relative =
      !std::is_base_of_v<callable_base, std::decay_t<Impl>>;
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
template <typename R, typename... Args>
class callable_ref : detail::callable_base<R, Args...> {
  using base = detail::callable_base<R, Args...>;
  template <typename, typename...> friend class callable;
  template <typename, typename...> friend class callable_shared;
  friend struct std::hash<callable_ref>;
  void *impl;
  typename base::vtable::invoke_t vtbl;
  callable_ref(void *impl, typename base::vtable const *vtbl) noexcept
      : impl(impl), vtbl(vtbl->invoke) {}

public:
  callable_ref() noexcept = default;
  template <typename Impl>
  explicit callable_ref(Impl *impl) noexcept
      : callable_ref(impl, &base::template vtable_for<Impl>) {}
  template <typename Impl>
  explicit callable_ref(std::unique_ptr<Impl> const &impl) noexcept
      : callable_ref(impl.get(), &base::template vtable_for<Impl>) {}
  template <typename Impl>
  explicit callable_ref(std::shared_ptr<Impl> const &impl) noexcept
      : callable_ref(impl.get(), &base::template vtable_for<Impl>) {}
  template <typename Impl,
            typename = std::enable_if_t<base::template not_relative<Impl> &&
                                        base::template not_smartptr<Impl>>>
  callable_ref(Impl &impl) noexcept
      : callable_ref(std::addressof(impl), &base::template vtable_for<Impl>) {}
  void swap(callable_ref &other) noexcept {
    std::swap(impl, other.impl);
    std::swap(vtbl, other.vtbl);
  }
  friend void swap(callable_ref &lhs, callable_ref &rhs) noexcept {
    lhs.swap(rhs);
  }
  friend bool operator==(callable_ref const &lhs,
                         callable_ref const &rhs) noexcept {
    return lhs.vtbl == rhs.vtbl && lhs.impl == rhs.impl;
  }

  R operator()(Args... args) const {
    return vtbl(impl, static_cast<Args &&>(args)...);
  }
};
template <typename R, typename... Args>
class callable : detail::callable_base<R, Args...> {
  using base = detail::callable_base<R, Args...>;
  friend struct std::hash<callable>;
  void *impl;
  typename base::vtable const *vtbl;

public:
  callable(callable const &) = delete;
  callable &operator=(callable const &) = delete;
  callable(callable &&other) noexcept : impl(other.impl), vtbl(other.vtbl) {
    other.impl = nullptr;
  }
  callable &operator=(callable &&other) noexcept {
    auto tmp(std::move(other));
    tmp.swap(*this);
    return *this;
  }
  template <typename Impl>
  explicit callable(Impl *impl) noexcept
      : impl(impl), vtbl(&base::template vtable_for<Impl>) {}
  template <typename Impl>
  explicit callable(std::unique_ptr<Impl> impl) noexcept
      : impl(impl.release()), vtbl(&base::template vtable_for<Impl>) {}
  template <typename Impl,
            typename = std::enable_if_t<base::template not_relative<Impl>>>
  callable(Impl &&impl)
      : impl(new std::remove_reference_t<Impl>(std::forward<Impl>(impl))),
        vtbl(&base::template vtable_for<std::remove_reference_t<Impl>>) {}
  ~callable() {
    vtbl->destroy(impl);
    impl = nullptr;
  }
  operator callable_ref<R, Args...>() const { return {impl, vtbl}; }
  void swap(callable &other) noexcept {
    std::swap(impl, other.impl);
    std::swap(vtbl, other.vtbl);
  }
  friend void swap(callable &lhs, callable &rhs) noexcept { lhs.swap(rhs); }
  friend bool operator==(callable const &lhs, callable const &rhs) noexcept {
    return lhs.vtbl == rhs.vtbl && lhs.impl == rhs.impl;
  }

  R operator()(Args... args) const {
    return vtbl->invoke(impl, static_cast<Args &&>(args)...);
  }
};
template <typename R, typename... Args>
class callable_shared : detail::callable_base<R, Args...> {
  using base = detail::callable_base<R, Args...>;
  friend struct std::hash<callable_shared>;
  std::shared_ptr<void> impl;
  typename base::vtable const *vtbl;

public:
  template <typename Impl>
  explicit callable_shared(Impl *impl)
      : impl(impl), vtbl(&base::template vtable_for<Impl>) {}
  template <typename Impl>
  explicit callable_shared(std::unique_ptr<Impl> impl) noexcept
      : impl(impl.release()), vtbl(&base::template vtable_for<Impl>) {}
  template <typename Impl>
  explicit callable_shared(std::shared_ptr<Impl> impl) noexcept
      : impl(impl), vtbl(&base::template vtable_for<Impl>) {}
  template <typename Impl,
            typename = std::enable_if_t<base::template not_relative<Impl>>>
  callable_shared(Impl &&impl)
      : impl(std::make_shared<std::remove_reference_t<Impl>>(
            std::forward<Impl>(impl))),
        vtbl(&base::template vtable_for<std::remove_reference_t<Impl>>) {}
  operator callable_ref<R, Args...>() const { return {impl.get(), vtbl}; }
  void swap(callable_shared &other) noexcept {
    impl.swap(other.impl);
    std::swap(vtbl, other.vtbl);
  }
  friend void swap(callable_shared &lhs, callable_shared &rhs) noexcept {
    lhs.swap(rhs);
  }
  friend bool operator==(callable_shared const &lhs,
                         callable_shared const &rhs) noexcept {
    return lhs.vtbl == rhs.vtbl && lhs.impl == rhs.impl;
  }

  R operator()(Args... args) const {
    return vtbl->invoke(impl.get(), static_cast<Args &&>(args)...);
  }
};
} // namespace jduck::gen_trait::sample

namespace std {
template <typename R, typename... Args>
struct hash<jduck::gen_trait::sample::callable_ref<R, Args...>> {
  size_t operator()(jduck::gen_trait::sample::callable_ref<R, Args...> const &v)
      const noexcept {
    return std::hash<void *>()(v.impl);
  }
};
template <typename R, typename... Args>
struct hash<jduck::gen_trait::sample::callable<R, Args...>> {
  size_t operator()(
      jduck::gen_trait::sample::callable<R, Args...> const &v) const noexcept {
    return std::hash<void *>()(v.impl);
  }
};
template <typename R, typename... Args>
struct hash<jduck::gen_trait::sample::callable_shared<R, Args...>> {
  size_t operator()(jduck::gen_trait::sample::callable_shared<R, Args...> const
                        &v) const noexcept {
    return std::hash<void *>()(v.impl.get());
  }
};
} // namespace std
#endif // INCLUDE_ee38b9e9b823fab6fd28013aa63d40aaf378a051
