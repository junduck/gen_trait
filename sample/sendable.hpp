#ifndef INCLUDE_e28c93d6762249772fbd9f67e47c9a533d7ba8fa
#define INCLUDE_e28c93d6762249772fbd9f67e47c9a533d7ba8fa

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace jduck::gen_trait::sample {
namespace detail {
struct sendable_base {
  struct vtable_base {
    using _gentrait_fn0_t = bool (*)(void *, std::string const &);
    _gentrait_fn0_t _gentrait_fn0;
    using _gentrait_fn1_t = bool (*)(void *, std::vector<char>);
    _gentrait_fn1_t _gentrait_fn1;
    bool operator==(vtable_base const &) const = default;
  };
  struct vtable : vtable_base {
    void (*_gentrait_destroy)(void *);
  };
  template <typename _GENTRAIT_IMPL>
  struct vtable_impl {
    static bool _gentrait_fn0(void *_gentrait_impl, std::string const &msg) { return static_cast<_GENTRAIT_IMPL *>(_gentrait_impl)->send(msg); }
    static bool _gentrait_fn1(void *_gentrait_impl, std::vector<char> msg) { return static_cast<_GENTRAIT_IMPL *>(_gentrait_impl)->send(std::move(msg)); }
    static void _gentrait_destroy(void *impl) { delete static_cast<_GENTRAIT_IMPL *>(impl); }
  };
  template <typename _GENTRAIT_IMPL>
  constexpr static vtable vtable_for{
      vtable_impl<_GENTRAIT_IMPL>::_gentrait_fn0,
      vtable_impl<_GENTRAIT_IMPL>::_gentrait_fn1,
      vtable_impl<_GENTRAIT_IMPL>::_gentrait_destroy,
  };
  template <typename _GENTRAIT_IMPL>
  constexpr static bool not_relative = !std::is_base_of_v<sendable_base, std::decay_t<_GENTRAIT_IMPL>>;
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
class sendable_ref : detail::sendable_base {
  using base = detail::sendable_base;
  friend class sendable;
  friend class sendable_shared;
  friend struct std::hash<sendable_ref>;
  void *_gentrait_impl;
  typename base::vtable_base const *_gentrait_vtbl;
  sendable_ref(void *impl, typename base::vtable const *vtbl) noexcept : _gentrait_impl(impl), _gentrait_vtbl(vtbl) {}

public:
  sendable_ref() noexcept = default;
  template <typename _GENTRAIT_IMPL>
  explicit sendable_ref(_GENTRAIT_IMPL *impl) noexcept : sendable_ref(impl, &base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL>
  explicit sendable_ref(std::unique_ptr<_GENTRAIT_IMPL> const &impl) noexcept : sendable_ref(impl.get(), &base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL>
  explicit sendable_ref(std::shared_ptr<_GENTRAIT_IMPL> const &impl) noexcept : sendable_ref(impl.get(), &base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL,
            typename = std::enable_if_t<base::template not_relative<_GENTRAIT_IMPL> &&
                                        base::template not_smartptr<_GENTRAIT_IMPL>>>
  sendable_ref(_GENTRAIT_IMPL &impl) noexcept : sendable_ref(std::addressof(impl), &base::template vtable_for<_GENTRAIT_IMPL>) {}
  void swap(sendable_ref &other) noexcept {
    std::swap(_gentrait_impl, other._gentrait_impl);
    std::swap(_gentrait_vtbl, other._gentrait_vtbl);
  }
  friend void swap(sendable_ref &lhs, sendable_ref &rhs) noexcept { lhs.swap(rhs); }
  friend bool operator==(sendable_ref const &lhs, sendable_ref const &rhs) noexcept { return lhs._gentrait_vtbl == rhs._gentrait_vtbl && lhs._gentrait_impl == rhs._gentrait_impl; }

  bool send(std::string const &msg) { return _gentrait_vtbl->_gentrait_fn0(_gentrait_impl, msg); }
  bool send(std::vector<char> msg) { return _gentrait_vtbl->_gentrait_fn1(_gentrait_impl, std::move(msg)); }
};
class sendable : detail::sendable_base {
  using base = detail::sendable_base;
  friend struct std::hash<sendable>;
  void *_gentrait_impl;
  typename base::vtable const *_gentrait_vtbl;

public:
  sendable(sendable const &) = delete;
  sendable &operator=(sendable const &) = delete;
  sendable(sendable &&other) noexcept : _gentrait_impl(other._gentrait_impl), _gentrait_vtbl(other._gentrait_vtbl) { other._gentrait_impl = nullptr; }
  sendable &operator=(sendable &&other) noexcept {
    auto tmp(std::move(other));
    tmp.swap(*this);
    return *this;
  }
  template <typename _GENTRAIT_IMPL>
  explicit sendable(_GENTRAIT_IMPL *impl) noexcept : _gentrait_impl(impl), _gentrait_vtbl(&base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL>
  explicit sendable(std::unique_ptr<_GENTRAIT_IMPL> impl) noexcept : _gentrait_impl(impl.release()), _gentrait_vtbl(&base::template vtable_for<_GENTRAIT_IMPL>) {}
  template <typename _GENTRAIT_IMPL, typename = std::enable_if_t<base::template not_relative<_GENTRAIT_IMPL>>>
  sendable(_GENTRAIT_IMPL &&impl) : _gentrait_impl(new std::remove_reference_t<_GENTRAIT_IMPL>(std::forward<_GENTRAIT_IMPL>(impl))),
                                    _gentrait_vtbl(&base::template vtable_for<std::remove_reference_t<_GENTRAIT_IMPL>>) {}
  ~sendable() {
    _gentrait_vtbl->_gentrait_destroy(_gentrait_impl);
    _gentrait_impl = nullptr;
  }
  operator sendable_ref() const { return {_gentrait_impl, _gentrait_vtbl}; }
  void swap(sendable &other) noexcept {
    std::swap(_gentrait_impl, other._gentrait_impl);
    std::swap(_gentrait_vtbl, other._gentrait_vtbl);
  }
  friend void swap(sendable &lhs, sendable &rhs) noexcept { lhs.swap(rhs); }
  friend bool operator==(sendable const &lhs, sendable const &rhs) noexcept { return lhs._gentrait_vtbl == rhs._gentrait_vtbl && lhs._gentrait_impl == rhs._gentrait_impl; }

  bool send(std::string const &msg) { return _gentrait_vtbl->_gentrait_fn0(_gentrait_impl, msg); }
  bool send(std::vector<char> msg) { return _gentrait_vtbl->_gentrait_fn1(_gentrait_impl, std::move(msg)); }
};

} // namespace jduck::gen_trait::sample

namespace std {
template <>
struct hash<jduck::gen_trait::sample::sendable_ref> {
  size_t operator()(jduck::gen_trait::sample::sendable_ref const &v) const noexcept {
    return std::hash<void *>()(v._gentrait_impl);
  }
};
template <>
struct hash<jduck::gen_trait::sample::sendable> {
  size_t operator()(jduck::gen_trait::sample::sendable const &v) const noexcept {
    return std::hash<void *>()(v._gentrait_impl);
  }
};

} // namespace std
#endif // INCLUDE_e28c93d6762249772fbd9f67e47c9a533d7ba8fa
