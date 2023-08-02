#ifndef INCLUDE_a3e61ceb609a73de093fe1c1276080e2d252fde5
#define INCLUDE_a3e61ceb609a73de093fe1c1276080e2d252fde5

#include <functional>
#include <memory>

namespace jduck::gen_trait::sample {
namespace detail {
template <typename R, typename... Args> struct callable_base {
  struct vtable {
    using _gen_trait__gen_trait_invoke_vtbl_t = R (*)(void *, Args...);
    _gen_trait__gen_trait_invoke_vtbl_t _gen_trait_invoke;
    void (*_gen_trait_destroy)(void *);
  };
  template <typename _GEN_TRAIT_TMPL_Impl> struct vtable_impl {
    static R _gen_trait_invoke(void *_gen_trait_impl, Args... args) {
      return static_cast<_GEN_TRAIT_TMPL_Impl *>(_gen_trait_impl)
          ->
          operator()(static_cast<Args &&>(args)...);
    }
    static void _gen_trait_destroy(void *_gen_trait_impl) {
      delete static_cast<_GEN_TRAIT_TMPL_Impl *>(_gen_trait_impl);
    }
  };
  template <typename _GEN_TRAIT_TMPL_Impl>
  constexpr static vtable vtable_for{
      vtable_impl<_GEN_TRAIT_TMPL_Impl>::_gen_trait_invoke,
      vtable_impl<_GEN_TRAIT_TMPL_Impl>::_gen_trait_destroy,
  };
  template <typename _GEN_TRAIT_TMPL_Impl>
  constexpr static bool not_relative =
      !std::is_base_of_v<callable_base, std::decay_t<_GEN_TRAIT_TMPL_Impl>>;
  template <typename _GEN_TRAIT_TMPL_T,
            template <typename...> typename _GEN_TRAIT_TMPL_TA>
  struct is_specialization_of : std::false_type {};
  template <template <typename...> typename _GEN_TRAIT_TMPL_T,
            typename... _GEN_TRAIT_TMPL_TA>
  struct is_specialization_of<_GEN_TRAIT_TMPL_T<_GEN_TRAIT_TMPL_TA...>,
                              _GEN_TRAIT_TMPL_T> : std::true_type {};
  template <typename _GEN_TRAIT_TMPL_T>
  constexpr static bool not_smartptr =
      !(is_specialization_of<_GEN_TRAIT_TMPL_T, std::shared_ptr>::value ||
        is_specialization_of<_GEN_TRAIT_TMPL_T, std::unique_ptr>::value);
};
} // namespace detail
template <typename R, typename... Args>
class callable_ref : detail::callable_base<R, Args...> {
  using base = detail::callable_base<R, Args...>;
  template <typename, typename...> friend class callable;
  template <typename, typename...> friend class callable_shared;
  friend struct std::hash<callable_ref>;
  void *_gen_trait_impl;
  typename base::vtable::_gen_trait__gen_trait_invoke_vtbl_t _gen_trait_vtbl;
  callable_ref(void *impl, typename base::vtable const *vtbl) noexcept
      : _gen_trait_impl(impl), _gen_trait_vtbl(vtbl->_gen_trait_invoke) {}

public:
  callable_ref() noexcept = default;
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit callable_ref(_GEN_TRAIT_TMPL_Impl *impl) noexcept
      : callable_ref(impl, &base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit callable_ref(
      std::unique_ptr<_GEN_TRAIT_TMPL_Impl> const &impl) noexcept
      : callable_ref(impl.get(),
                     &base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit callable_ref(
      std::shared_ptr<_GEN_TRAIT_TMPL_Impl> const &impl) noexcept
      : callable_ref(impl.get(),
                     &base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl,
            typename = std::enable_if_t<
                base::template not_relative<_GEN_TRAIT_TMPL_Impl> &&
                base::template not_smartptr<_GEN_TRAIT_TMPL_Impl>>>
  callable_ref(_GEN_TRAIT_TMPL_Impl &impl) noexcept
      : callable_ref(std::addressof(impl),
                     &base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  void swap(callable_ref &other) noexcept {
    std::swap(_gen_trait_impl, other._gen_trait_impl);
    std::swap(_gen_trait_vtbl, other._gen_trait_vtbl);
  }
  friend void swap(callable_ref &lhs, callable_ref &rhs) noexcept {
    lhs.swap(rhs);
  }
  friend bool operator==(callable_ref const &lhs,
                         callable_ref const &rhs) noexcept {
    return lhs._gen_trait_vtbl == rhs._gen_trait_vtbl &&
           lhs._gen_trait_impl == rhs._gen_trait_impl;
  }

  R operator()(Args... args) const {
    return _gen_trait_vtbl(_gen_trait_impl, static_cast<Args &&>(args)...);
  }
};
template <typename R, typename... Args>
class callable : detail::callable_base<R, Args...> {
  using base = detail::callable_base<R, Args...>;
  friend struct std::hash<callable>;
  void *_gen_trait_impl;
  typename base::vtable const *_gen_trait_vtbl;

public:
  callable(callable const &) = delete;
  callable &operator=(callable const &) = delete;
  callable(callable &&other) noexcept
      : _gen_trait_impl(other._gen_trait_impl),
        _gen_trait_vtbl(other._gen_trait_vtbl) {
    other._gen_trait_impl = nullptr;
  }
  callable &operator=(callable &&other) noexcept {
    auto tmp(std::move(other));
    tmp.swap(*this);
    return *this;
  }
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit callable(_GEN_TRAIT_TMPL_Impl *impl) noexcept
      : _gen_trait_impl(impl),
        _gen_trait_vtbl(&base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit callable(std::unique_ptr<_GEN_TRAIT_TMPL_Impl> impl) noexcept
      : _gen_trait_impl(impl.release()),
        _gen_trait_vtbl(&base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl,
            typename = std::enable_if_t<
                base::template not_relative<_GEN_TRAIT_TMPL_Impl>>>
  callable(_GEN_TRAIT_TMPL_Impl &&impl)
      : _gen_trait_impl(new std::remove_reference_t<_GEN_TRAIT_TMPL_Impl>(
            std::forward<_GEN_TRAIT_TMPL_Impl>(impl))),
        _gen_trait_vtbl(&base::template vtable_for<
                        std::remove_reference_t<_GEN_TRAIT_TMPL_Impl>>) {}
  ~callable() {
    _gen_trait_vtbl->_gen_trait_destroy(_gen_trait_impl);
    _gen_trait_impl = nullptr;
  }
  operator callable_ref<R, Args...>() const {
    return {_gen_trait_impl, _gen_trait_vtbl};
  }
  void swap(callable &other) noexcept {
    std::swap(_gen_trait_impl, other._gen_trait_impl);
    std::swap(_gen_trait_vtbl, other._gen_trait_vtbl);
  }
  friend void swap(callable &lhs, callable &rhs) noexcept { lhs.swap(rhs); }
  friend bool operator==(callable const &lhs, callable const &rhs) noexcept {
    return lhs._gen_trait_vtbl == rhs._gen_trait_vtbl &&
           lhs._gen_trait_impl == rhs._gen_trait_impl;
  }

  R operator()(Args... args) const {
    return _gen_trait_vtbl->_gen_trait_invoke(_gen_trait_impl,
                                              static_cast<Args &&>(args)...);
  }
};
template <typename R, typename... Args>
class callable_shared : detail::callable_base<R, Args...> {
  using base = detail::callable_base<R, Args...>;
  friend struct std::hash<callable_shared>;
  std::shared_ptr<void> _gen_trait_impl;
  typename base::vtable const *_gen_trait_vtbl;

public:
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit callable_shared(_GEN_TRAIT_TMPL_Impl *impl)
      : _gen_trait_impl(impl),
        _gen_trait_vtbl(&base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit callable_shared(std::unique_ptr<_GEN_TRAIT_TMPL_Impl> impl) noexcept
      : _gen_trait_impl(impl.release()),
        _gen_trait_vtbl(&base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl>
  explicit callable_shared(std::shared_ptr<_GEN_TRAIT_TMPL_Impl> impl) noexcept
      : _gen_trait_impl(impl),
        _gen_trait_vtbl(&base::template vtable_for<_GEN_TRAIT_TMPL_Impl>) {}
  template <typename _GEN_TRAIT_TMPL_Impl,
            typename = std::enable_if_t<
                base::template not_relative<_GEN_TRAIT_TMPL_Impl>>>
  callable_shared(_GEN_TRAIT_TMPL_Impl &&impl)
      : _gen_trait_impl(
            std::make_shared<std::remove_reference_t<_GEN_TRAIT_TMPL_Impl>>(
                std::forward<_GEN_TRAIT_TMPL_Impl>(impl))),
        _gen_trait_vtbl(&base::template vtable_for<
                        std::remove_reference_t<_GEN_TRAIT_TMPL_Impl>>) {}
  operator callable_ref<R, Args...>() const {
    return {_gen_trait_impl.get(), _gen_trait_vtbl};
  }
  void swap(callable_shared &other) noexcept {
    _gen_trait_impl.swap(other._gen_trait_impl);
    std::swap(_gen_trait_vtbl, other._gen_trait_vtbl);
  }
  friend void swap(callable_shared &lhs, callable_shared &rhs) noexcept {
    lhs.swap(rhs);
  }
  friend bool operator==(callable_shared const &lhs,
                         callable_shared const &rhs) noexcept {
    return lhs._gen_trait_vtbl == rhs._gen_trait_vtbl &&
           lhs._gen_trait_impl == rhs._gen_trait_impl;
  }

  R operator()(Args... args) const {
    return _gen_trait_vtbl->_gen_trait_invoke(_gen_trait_impl.get(),
                                              static_cast<Args &&>(args)...);
  }
};
} // namespace jduck::gen_trait::sample

namespace std {
template <typename R, typename... Args>
struct hash<jduck::gen_trait::sample::callable_ref<R, Args...>> {
  size_t operator()(jduck::gen_trait::sample::callable_ref<R, Args...> const &v)
      const noexcept {
    return std::hash<void *>()(v._gen_trait_impl);
  }
};
template <typename R, typename... Args>
struct hash<jduck::gen_trait::sample::callable<R, Args...>> {
  size_t operator()(
      jduck::gen_trait::sample::callable<R, Args...> const &v) const noexcept {
    return std::hash<void *>()(v._gen_trait_impl);
  }
};
template <typename R, typename... Args>
struct hash<jduck::gen_trait::sample::callable_shared<R, Args...>> {
  size_t operator()(jduck::gen_trait::sample::callable_shared<R, Args...> const
                        &v) const noexcept {
    return std::hash<void *>()(v._gen_trait_impl.get());
  }
};
} // namespace std
#endif // INCLUDE_a3e61ceb609a73de093fe1c1276080e2d252fde5
